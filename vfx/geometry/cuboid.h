// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_GEOMETRY_CUBOID_H_
#define VFX_GEOMETRY_CUBOID_H_

#include "vfx/geometry/quad.h"
#include "vfx/geometry/triangle_strip.h"

namespace vfx {

class Cuboid {
 public:
  Cuboid() { }
  Cuboid(const Quad& top, const Quad& bottom) {
    top_ = top;
    bottom_ = bottom;
  }

  const Quad& top() const { return top_; }
  const Quad& bottom() const { return bottom_; }

  void set_top(const Quad& top) { top_ = top; }
  void set_bottom(const Quad& bottom) { bottom_ = bottom; }

  TriangleStrip Tessellate();

 private:
  Quad top_;
  Quad bottom_;
};

}  // namespace vfx

#endif  // VFX_GEOMETRY_CUBOID_H_
