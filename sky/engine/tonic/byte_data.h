// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_ENGINE_TONIC_BYTE_DATA_H_
#define SKY_ENGINE_TONIC_BYTE_DATA_H_

#include <vector>

#include "dart/runtime/include/dart_api.h"
#include "sky/engine/tonic/dart_converter.h"

namespace blink {

class ByteData {
 public:
  explicit ByteData(Dart_Handle list);
  ByteData(ByteData&& other);
  ByteData();
  ~ByteData();

  const void* data() const { return data_; }
  size_t length_in_bytes() const { return length_in_bytes_; }
  Dart_Handle dart_handle() const { return dart_handle_; }

  std::vector<char> Copy() const;
  void Release() const;

 private:
  mutable void* data_;
  intptr_t length_in_bytes_;
  Dart_Handle dart_handle_;

  ByteData(const ByteData& other) = delete;
};

template <>
struct DartConverter<ByteData> {
  static void SetReturnValue(Dart_NativeArguments args, ByteData val);
  static ByteData FromArguments(Dart_NativeArguments args,
                                int index,
                                Dart_Handle& exception);
};

} // namespace blink

#endif  // SKY_ENGINE_TONIC_BYTE_DATA_H_
