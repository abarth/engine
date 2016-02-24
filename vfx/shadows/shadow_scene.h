// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_SHADOWS_SHADOW_SCENE_H_
#define VFX_SHADOWS_SHADOW_SCENE_H_

#include <vector>

#include "vfx/geometry/color.h"
#include "vfx/geometry/element_array.h"
#include "vfx/geometry/quad.h"
#include "vfx/geometry/triangle_strip.h"

namespace vfx {

class ShadowScene {
 public:
  struct Object {
    Quad quad;
    Color color;
  };

  struct Vertex {
    Point point;
    Color color;
  };

  explicit ShadowScene(std::vector<Object> objects);
  ~ShadowScene();

  ShadowScene(const ShadowScene& other) = delete;
  ShadowScene(ShadowScene&& other);

  ShadowScene& operator=(const ShadowScene& other) = delete;
  ShadowScene& operator=(ShadowScene&& other);

  ElementArray<Vertex> BuildGeometry();
  TriangleStrip BuildShadowVolume();

 private:
  Point light_;
  std::vector<Object> objects_;
};

}  // namespace vfx

#endif  // VFX_SHADOWS_SHADOW_SCENE_H_
