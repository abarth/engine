// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/shadows/shadow_scene.h"

#include <memory>

namespace vfx {

ShadowScene::ShadowScene(std::vector<Object> objects)
  : objects_(std::move(objects)) {
}

ShadowScene::~ShadowScene() {
}

ElementArray<ShadowScene::Vertex> ShadowScene::BuildElementArray() {
  ElementArray<Vertex> array;

  for (const Object& object : objects_) {
    array.AddQuad(Vertex{ object.quad.p1(), object.color },
                  Vertex{ object.quad.p2(), object.color },
                  Vertex{ object.quad.p3(), object.color },
                  Vertex{ object.quad.p4(), object.color });
  }

  return array;
}

}  // namespace vfx
