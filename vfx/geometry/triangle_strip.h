// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_GEOMETRY_TRIANGLE_STRIP_H_
#define VFX_GEOMETRY_TRIANGLE_STRIP_H_

#include <memory>
#include <vector>

#include "vfx/geometry/point.h"

namespace vfx {

class TriangleStrip {
 public:
  TriangleStrip();
  ~TriangleStrip();
  explicit TriangleStrip(std::vector<Point> data);

  TriangleStrip(const TriangleStrip& other) = delete;
  TriangleStrip(TriangleStrip&& other);

  TriangleStrip& operator=(const TriangleStrip& other) = delete;
  TriangleStrip& operator=(TriangleStrip&& other);

  const Point* data() const { return data_.data(); }
  size_t size() const { return data_.size(); }

 private:
  std::vector<Point> data_;
};

}  // namespace vfx

#endif  // VFX_GEOMETRY_TRIANGLE_STRIP_H_
