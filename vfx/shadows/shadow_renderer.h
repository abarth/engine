// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_SHADOWS_SHADOW_RENDERER_H_
#define VFX_SHADOWS_SHADOW_RENDERER_H_

#include <memory>

#include "vfx/geometry/element_array.h"
#include "vfx/geometry/triangle_strip.h"
#include "vfx/gl/element_binding.h"
#include "vfx/shadows/shadow_scene.h"
#include "vfx/shadows/solid_quad_program.h"

namespace vfx {

class ShadowRenderer {
 public:
  explicit ShadowRenderer(ShadowScene scene);
  ~ShadowRenderer();

  void PrepareToDraw();
  void Draw(int width, int height);

 private:
  ShadowScene scene_;

  std::unique_ptr<SolidQuadProgram> quad_program_;
  ElementArray<ShadowScene::Vertex> geometry_;
  TriangleStrip shadow_volume_;
  ElementBinding geometry_binding_;
};

}  // namespace vfx

#endif  // VFX_SHADOWS_SHADOW_RENDERER_H_
