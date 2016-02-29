// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/shadows/shadow_scene.h"

#include <memory>

#include "vfx/geometry/cuboid.h"

namespace vfx {
namespace {

const float kFar = 50.0f;

ColorProgram::Vertex CreateUmbraVertex(const Point& point) {
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

void ShadowScene::ComputeShadow() {
  shadow_.Init(objects_[0].quad, light_, kFar);
}

ElementArrayBuffer<ColorProgram::Vertex> ShadowScene::BuildGeometry() {
  ElementArrayBuffer<ColorProgram::Vertex> buffer;

  for (const Object& object : objects_) {
    buffer.AddQuad({ object.quad.p1(), object.color },
                   { object.quad.p2(), object.color },
                   { object.quad.p3(), object.color },
                   { object.quad.p4(), object.color });
  }

  return buffer;
}

ArrayBuffer<ColorProgram::Vertex> ShadowScene::BuildUmbra() {
  if (objects_.empty())
    return ArrayBuffer<ColorProgram::Vertex>();
  const Quad& front = objects_[0].quad;
  const Triangle* shadow = shadow_.shadow();
  Cuboid umbra(front, Quad(shadow[0].p2(),
                           shadow[1].p2(),
                           shadow[2].p2(),
                           shadow[3].p2()));


  return ArrayBuffer<ColorProgram::Vertex>(
      GL_TRIANGLE_STRIP, umbra.Tessellate(CreateUmbraVertex));
}

ElementArrayBuffer<PenumbraProgram::Vertex> ShadowScene::BuildPenumbra() {
  ElementArrayBuffer<PenumbraProgram::Vertex> buffer;
  const Triangle* shadow = shadow_.shadow();

  for (int i = 0; i < 4; ++i) {
    int j = (i + 1) % 4;
    size_t base = buffer.vertex_count();
    Wedge penumbra(shadow[i], shadow[j]);

    // Sides
    buffer.AddTriangle({ shadow[i].p1(), penumbra },
                       { shadow[i].p2(), penumbra },
                       { shadow[i].p3(), penumbra });

    buffer.AddTriangle({ shadow[j].p1(), penumbra },
                       { shadow[j].p3(), penumbra },
                       { shadow[j].p2(), penumbra });

    // Front
    buffer.AddQuadIndices(base + 0, base + 2, base + 4, base + 3);

    // Back
    buffer.AddQuadIndices(base + 0, base + 3, base + 5, base + 1);
  }

  return buffer;
}

}  // namespace vfx
