// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_SHADOWS_SHADOW_SCENE_H_
#define VFX_SHADOWS_SHADOW_SCENE_H_

#include <vector>

#include "base/macros.h"
#include "vfx/geometry/color.h"
#include "vfx/geometry/quad.h"
#include "vfx/geometry/sphere.h"
#include "vfx/gl/array_buffer.h"
#include "vfx/gl/color_program.h"
#include "vfx/gl/element_array_buffer.h"
#include "vfx/shadows/penumbra_program.h"

namespace vfx {

class ShadowScene {
 public:
  struct Object {
    Quad quad;
    Color color;
  };

  explicit ShadowScene(std::vector<Object> objects);
  ~ShadowScene();

  ShadowScene(ShadowScene&& other);
  ShadowScene& operator=(ShadowScene&& other);

  void set_light(Sphere light) { light_ = light; }

  ElementArrayBuffer<ColorProgram::Vertex> BuildGeometry();
  // ArrayBuffer<ColorProgram::Vertex> BuildShadowVolume();
  ElementArrayBuffer<PenumbraProgram::Vertex> BuildPenumbra();

 private:
  Sphere light_;
  std::vector<Object> objects_;

  DISALLOW_COPY_AND_ASSIGN(ShadowScene);
};

}  // namespace vfx

#endif  // VFX_SHADOWS_SHADOW_SCENE_H_
