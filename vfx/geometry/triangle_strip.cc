// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/geometry/triangle_strip.h"

namespace vfx {

TriangleStrip::TriangleStrip() {
}

TriangleStrip::~TriangleStrip() {
}

TriangleStrip::TriangleStrip(std::vector<Point> data)
  : data_(std::move(data)) {
}

TriangleStrip::TriangleStrip(TriangleStrip&& other)
  : data_(std::move(other.data_)) {
}

TriangleStrip& TriangleStrip::operator=(TriangleStrip&& other) {
  data_ = std::move(other.data_);
  return *this;
}

}  // namespace vfx
