// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/shadows/shadows.h"

#include <memory>
#include <vector>

#include "vfx/shadows/shadow_renderer.h"
#include "vfx/shadows/shadow_scene.h"

namespace flow {
namespace {

static vfx::ShadowRenderer* renderer = nullptr;

} // namespace

void DrawShadowTest(int width, int height, unsigned int fbo) {
  if (!renderer) {
    std::vector<vfx::ShadowScene::Object> objects;
    objects.push_back({
      vfx::Quad({
        vfx::Point({  1, -1,  0 }),
        vfx::Point({  1,  1,  0 }),
        vfx::Point({ -1,  1,  0 }),
        vfx::Point({ -1, -1,  0 }),
      }),
      vfx::Color(0, 1, 0, 1),
    });
    objects.push_back({
      vfx::Quad({
        vfx::Point({  2, -2, -1 }),
        vfx::Point({  2,  2, -1 }),
        vfx::Point({ -2,  2, -1 }),
        vfx::Point({ -2, -2, -1 }),
      }),
      vfx::Color(0, 1, 1, 1),
    });
    objects.push_back({
      vfx::Quad({
        vfx::Point({  10, -10, -2 }),
        vfx::Point({  10,  10, -2 }),
        vfx::Point({ -10,  10, -2 }),
        vfx::Point({ -10, -10, -2 }),
      }),
      vfx::Color(1, 1, 1, 1),
    });

    vfx::ShadowScene scene(std::move(objects));
    scene.set_light(vfx::Sphere(vfx::Point(-2.0f, 4.0f, 5.0f), 0.2));
    renderer = new vfx::ShadowRenderer(std::move(scene));
    renderer->PrepareToDraw();
  }

  renderer->Draw(gfx::Size(width, height));
}

}  // namespace flow
