// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/geometry/cuboid.h"

namespace vfx {

TriangleStrip Cuboid::Tessellate() {
  std::vector<Point> points;
  points.reserve(14);

  points.push_back(top_[0]);
  points.push_back(top_[1]);
  points.push_back(top_[3]);
  points.push_back(top_[2]);
  points.push_back(bottom_[2]);
  points.push_back(top_[1]);
  points.push_back(bottom_[1]);
  points.push_back(top_[0]);
  points.push_back(bottom_[0]);
  points.push_back(top_[3]);
  points.push_back(bottom_[3]);
  points.push_back(bottom_[2]);
  points.push_back(bottom_[0]);
  points.push_back(bottom_[1]);

  return TriangleStrip(std::move(points));
}

}  // namespace
