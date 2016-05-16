// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/core/compositing/buffer_decoder.h"

#include <memory>
#include "base/logging.h"
#include "sky/engine/core/painting/Matrix.h"

namespace blink {

BufferDecoder::BufferDecoder(const void* buffer, size_t length)
    : offset_(0),
      length_(length),
      buffer_(static_cast<const char*>(buffer))
{
}

BufferDecoder::~BufferDecoder()
{
}

int64_t BufferDecoder::ReadInt64() {
  size_t readOffset = offset_;
  offset_ += 8;
  CHECK(offset_ <= length_) << "Offset: " << offset_ << " Length: " << length_;
  return *reinterpret_cast<const int64_t*>(buffer_ + readOffset);
}

double BufferDecoder::ReadFloat64() {
  size_t readOffset = offset_;
  offset_ += 8;
  CHECK(offset_ <= length_) << "Offset: " << offset_ << " Length: " << length_;
  return *reinterpret_cast<const double*>(buffer_ + readOffset);
}

void BufferDecoder::Read4xFloat64(double* result) {
  constexpr int kReadCount = 4 * 8;
  size_t readOffset = offset_;
  offset_ += kReadCount;
  CHECK(offset_ <= length_) << "Offset: " << offset_ << " Length: " << length_;
  memcpy(result, buffer_ + readOffset, kReadCount);
}

void BufferDecoder::Read6xFloat64(double* result) {
  constexpr int kReadCount = 6 * 8;
  size_t readOffset = offset_;
  offset_ += kReadCount;
  CHECK(offset_ <= length_) << "Offset: " << offset_ << " Length: " << length_;
  memcpy(result, buffer_ + readOffset, kReadCount);
}

void BufferDecoder::Read16xFloat64(double* result) {
  constexpr int kReadCount = 16 * 8;
  size_t readOffset = offset_;
  offset_ += kReadCount;
  CHECK(offset_ <= length_) << "Offset: " << offset_ << " Length: " << length_;
  memcpy(result, buffer_ + readOffset, kReadCount);
}

SkMatrix BufferDecoder::ReadSkMatrix() {
  double matrix4[16];
  Read16xFloat64(matrix4);
  return toSkMatrix(matrix4);
}

SkRect BufferDecoder::ReadSkRect() {
  double rect[4];
  Read4xFloat64(rect);
  return SkRect::MakeLTRB(rect[0], rect[1], rect[2], rect[3]);
}

SkRRect BufferDecoder::ReadSkRRect(){
  double rrect[6];
  Read6xFloat64(rrect);
  SkRRect sk_rrect;
  sk_rrect.setRectXY(
      SkRect::MakeLTRB(rrect[0], rrect[1], rrect[2], rrect[3]),
      rrect[4], rrect[5]);
  return sk_rrect;
}

} // namespace blink
