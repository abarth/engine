// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/shadows/shadow_scene.h"

#include <memory>

#include "vfx/geometry/cuboid.h"

namespace vfx {
namespace {

const float kFar = 50.0f;

}  // namespace

ShadowScene::ShadowScene(std::vector<Object> objects)
  : objects_(std::move(objects)) {
}

ShadowScene::ShadowScene(ShadowScene&& other)
  : objects_(std::move(other.objects_)) {
}

ShadowScene::~ShadowScene() {
}

ShadowScene& ShadowScene::operator=(ShadowScene&& other) {
  objects_ = std::move(other.objects_);
  return *this;
}

ElementArrayBuffer<ShadowScene::Vertex> ShadowScene::BuildGeometry() {
  ElementArrayBuffer<Vertex> array;

  for (const Object& object : objects_) {
    array.AddQuad({ object.quad.p1(), object.color },
                  { object.quad.p2(), object.color },
                  { object.quad.p3(), object.color },
                  { object.quad.p4(), object.color });
  }

  return array;
}

ArrayBuffer<Point> ShadowScene::BuildShadowVolume() {
  if (objects_.empty())
    return ArrayBuffer<Point>();
  const Quad& front = objects_[0].quad;
  Quad back = front.ProjectDistanceFromSource(light_, kFar);
  return ArrayBuffer<Point>(GL_TRIANGLE_STRIP,
                            Cuboid(front, back).Tessellate());
}

}  // namespace vfx
