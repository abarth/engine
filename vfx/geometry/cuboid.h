// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_GEOMETRY_CUBOID_H_
#define VFX_GEOMETRY_CUBOID_H_

#include <vector>

#include "vfx/geometry/point.h"
#include "vfx/geometry/quad.h"

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

  template<typename Vertex>
  std::vector<Vertex> Tessellate(Vertex make_vertex(const Point& point)) {
    std::vector<Vertex> vertices;
    vertices.reserve(14);
    vertices.push_back(make_vertex(top_[0]));
    vertices.push_back(make_vertex(top_[1]));
    vertices.push_back(make_vertex(top_[3]));
    vertices.push_back(make_vertex(top_[2]));
    vertices.push_back(make_vertex(bottom_[2]));
    vertices.push_back(make_vertex(top_[1]));
    vertices.push_back(make_vertex(bottom_[1]));
    vertices.push_back(make_vertex(top_[0]));
    vertices.push_back(make_vertex(bottom_[0]));
    vertices.push_back(make_vertex(top_[3]));
    vertices.push_back(make_vertex(bottom_[3]));
    vertices.push_back(make_vertex(bottom_[2]));
    vertices.push_back(make_vertex(bottom_[0]));
    vertices.push_back(make_vertex(bottom_[1]));
    return vertices;
  }

 private:
  Quad top_;
  Quad bottom_;
};

}  // namespace vfx

#endif  // VFX_GEOMETRY_CUBOID_H_
