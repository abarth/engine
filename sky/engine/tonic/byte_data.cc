// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/tonic/byte_data.h"

#include "sky/engine/tonic/dart_error.h"

namespace blink {

ByteData::ByteData()
    : data_(nullptr), length_in_bytes_(0), dart_handle_(nullptr) {}

ByteData::ByteData(Dart_Handle list)
    : data_(nullptr), length_in_bytes_(0), dart_handle_(list) {
  if (Dart_IsNull(list))
    return;

  Dart_TypedData_Type type;
  Dart_TypedDataAcquireData(list, &type, &data_, &length_in_bytes_);
  DCHECK(!LogIfError(list));
  DCHECK(type == Dart_TypedData_kByteData);
}

ByteData::ByteData(ByteData&& other)
    : data_(other.data_),
      length_in_bytes_(other.length_in_bytes_),
      dart_handle_(other.dart_handle_) {
  other.data_ = nullptr;
  other.dart_handle_ = nullptr;
}

ByteData::~ByteData() {
  Release();
}

std::vector<char> ByteData::Copy() const {
  const char* ptr = static_cast<const char*>(data_);
  return std::vector<char>(ptr, ptr + length_in_bytes_);
}

void ByteData::Release() const {
  if (data_) {
    Dart_TypedDataReleaseData(dart_handle_);
    data_ = nullptr;
  }
}

ByteData DartConverter<ByteData>::FromArguments(
    Dart_NativeArguments args,
    int index,
    Dart_Handle& exception) {
  Dart_Handle data = Dart_GetNativeArgument(args, index);
  DCHECK(!LogIfError(data));
  return ByteData(data);
}

void DartConverter<ByteData>::SetReturnValue(Dart_NativeArguments args,
                                             ByteData val) {
  Dart_SetReturnValue(args, val.dart_handle());
}

} // namespace blink
