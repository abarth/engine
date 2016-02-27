// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/shadows/shadow_scene.h"

#include <memory>

#include "vfx/geometry/cuboid.h"

namespace vfx {
namespace {

const float kFar = 50.0f;

ColorProgram::Vertex CreateShadowVertex(const Point& point) {
  return ColorProgram::Vertex{ point, Color(0.0f, 0.0f, 0.0f, 1.0f)};
}

}  // namespace

ShadowScene::ShadowScene(std::vector<Object> objects)
  : objects_(std::move(objects)) {
}

ShadowScene::ShadowScene(ShadowScene&& other)
  : light_(other.light_),
    objects_(std::move(other.objects_)) {
}

ShadowScene::~ShadowScene() {
}

ShadowScene& ShadowScene::operator=(ShadowScene&& other) {
  if (this == &other)
    return *this;
  light_ = other.light_;
  objects_ = std::move(other.objects_);
  return *this;
}

ElementArrayBuffer<ColorProgram::Vertex> ShadowScene::BuildGeometry() {
  ElementArrayBuffer<ColorProgram::Vertex> array;

  for (const Object& object : objects_) {
    array.AddQuad({ object.quad.p1(), object.color },
                  { object.quad.p2(), object.color },
                  { object.quad.p3(), object.color },
                  { object.quad.p4(), object.color });
  }

  return array;
}

ArrayBuffer<ColorProgram::Vertex> ShadowScene::BuildShadowVolume() {
  if (objects_.empty())
    return ArrayBuffer<ColorProgram::Vertex>();
  const Quad& front = objects_[0].quad;
  Quad back = front.ProjectDistanceFromSource(light_, kFar);
  return ArrayBuffer<ColorProgram::Vertex>(
      GL_TRIANGLE_STRIP, Cuboid(front, back).Tessellate(CreateShadowVertex));
}

}  // namespace vfx
