// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_ENGINE_CORE_COMPOSITING_BUFFER_DECODER_H_
#define SKY_ENGINE_CORE_COMPOSITING_BUFFER_DECODER_H_

#include <stdint.h>

#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/include/core/SkRRect.h"

namespace blink {

class BufferDecoder {
 public:
  BufferDecoder(const void* buffer, size_t length);
  ~BufferDecoder();

  int64_t ReadInt64();
  double ReadFloat64();

  void Read4xFloat64(double* result);
  void Read6xFloat64(double* result);
  void Read16xFloat64(double* result);

  SkMatrix ReadSkMatrix();
  SkRect ReadSkRect();
  SkRRect ReadSkRRect();

  int ReadInt() { return static_cast<int>(ReadInt64()); }
  uint32_t ReadUInt32() { return static_cast<uint32_t>(ReadInt64()); }

  bool is_empty() const { return offset_ >= length_; }

 private:
  size_t offset_;
  size_t length_;
  const char* buffer_;
};

} // namespace blink

#endif  // SKY_ENGINE_CORE_COMPOSITING_BUFFER_DECODER_H_
