// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/lib/ui/window/platform_message_response_dart.h"

#include <utility>

#include "flutter/common/threads.h"
#include "lib/ftl/functional/make_copyable.h"
#include "lib/tonic/dart_state.h"
#include "lib/tonic/logging/dart_invoke.h"

namespace blink {
namespace {

constexpr size_t kCopyLimit = 4096;

void FinalizeByteData(void* isolate_callback_data,
                      Dart_WeakPersistentHandle handle,
                      void* peer) {
  std::vector<char>* heap_buffer = static_cast<std::vector<char>*>(peer);
  delete heap_buffer;
}

Dart_Handle MoveToByteData(std::vector<char> buffer) {
  if (buffer.empty())
    return Dart_Null();

  if (buffer.size() < kCopyLimit) {
    Dart_Handle data_handle =
        Dart_NewTypedData(Dart_TypedData_kByteData, buffer.size());
    if (Dart_IsError(data_handle))
      return data_handle;

    Dart_TypedData_Type type;
    void* data = nullptr;
    intptr_t num_bytes = 0;
    FTL_CHECK(!Dart_IsError(
        Dart_TypedDataAcquireData(data_handle, &type, &data, &num_bytes)));

    memcpy(data, buffer.data(), num_bytes);
    Dart_TypedDataReleaseData(data_handle);
    return data_handle;
  } else {
    std::vector<char>* heap_buffer = new std::vector<char>();
    heap_buffer->swap(buffer);
    Dart_Handle data_handle = Dart_NewExternalTypedData(
        Dart_TypedData_kByteData, heap_buffer->data(), heap_buffer->size());
    if (Dart_IsError(data_handle)) {
      delete heap_buffer;
      return data_handle;
    }
    Dart_NewWeakPersistentHandle(data_handle, heap_buffer, heap_buffer->size(),
                                 FinalizeByteData);
    return data_handle;
  }
}

}  // namespace

PlatformMessageResponseDart::PlatformMessageResponseDart(
    tonic::DartPersistentValue callback)
    : callback_(std::move(callback)) {}

PlatformMessageResponseDart::~PlatformMessageResponseDart() {
  if (!callback_.is_empty()) {
    Threads::UI()->PostTask(
        ftl::MakeCopyable([callback = std::move(callback_)]() mutable {
          callback.Clear();
        }));
  }
}

void PlatformMessageResponseDart::Complete(std::vector<char> data) {
  if (callback_.is_empty())
    return;
  FTL_DCHECK(!is_complete_);
  is_complete_ = true;
  Threads::UI()->PostTask(ftl::MakeCopyable(
      [ callback = std::move(callback_), data = std::move(data) ]() mutable {
        tonic::DartState* dart_state = callback.dart_state().get();
        if (!dart_state)
          return;
        tonic::DartState::Scope scope(dart_state);

        Dart_Handle byte_buffer;
        if (data.empty()) {
          byte_buffer = Dart_Null();
        } else {
          byte_buffer = MoveToByteData(std::move(data));
          DART_CHECK_VALID(byte_buffer);
        }

        tonic::DartInvoke(callback.Release(), {byte_buffer});
      }));
}

void PlatformMessageResponseDart::CompleteWithError() {
  // TODO(abarth): We should have a dedicated error pathway.
  Complete(std::vector<char>());
}

}  // namespace blink
