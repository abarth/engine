// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/shadows/shadow_scene.h"

#include <memory>

#include "vfx/geometry/cuboid.h"
#include "vfx/shadows/shadow_volume.h"

namespace vfx {
namespace {

const float kFar = 50.0f;

// ColorProgram::Vertex CreateShadowVertex(const Point& point) {
//   return ColorProgram::Vertex{ point, Color(0.0f, 0.0f, 0.0f, 1.0f)};
// }

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
  ElementArrayBuffer<ColorProgram::Vertex> buffer;

  for (const Object& object : objects_) {
    buffer.AddQuad({ object.quad.p1(), object.color },
                   { object.quad.p2(), object.color },
                   { object.quad.p3(), object.color },
                   { object.quad.p4(), object.color });
  }

  return buffer;
}

// ArrayBuffer<ColorProgram::Vertex> ShadowScene::BuildShadowVolume() {
//   if (objects_.empty())
//     return ArrayBuffer<ColorProgram::Vertex>();
//   const Quad& front = objects_[0].quad;
//   ShadowVolume shadow;
//   shadow.Init(front, light_, kFar);
//   return ArrayBuffer<ColorProgram::Vertex>(
//       GL_TRIANGLE_STRIP, shadow.umbra().Tessellate(CreateShadowVertex));

//   // Quad back = front.ProjectDistanceFromSource(light_.center(), kFar);
//   // return ArrayBuffer<ColorProgram::Vertex>(
//   //     GL_TRIANGLE_STRIP, Cuboid(front, back).Tessellate(CreateShadowVertex));

// }

ElementArrayBuffer<PenumbraProgram::Vertex> ShadowScene::BuildPenumbra() {
  if (objects_.empty())
    return ElementArrayBuffer<PenumbraProgram::Vertex>();
  const Quad& front = objects_[0].quad;

  ElementArrayBuffer<PenumbraProgram::Vertex> buffer;
  ShadowVolume shadow;
  shadow.Init(front, light_, kFar);

  const Triangle* penumbra = shadow.penumbra();

  Plane sides[4] = {
    penumbra[0].GetPlane(),
    penumbra[1].GetPlane(),
    penumbra[2].GetPlane(),
    penumbra[3].GetPlane(),
  };

  Plane backs[4] = {
    Plane(penumbra[0].p1(), penumbra[1].p1(), penumbra[1].p2()),
    Plane(penumbra[1].p1(), penumbra[2].p1(), penumbra[2].p2()),
    Plane(penumbra[2].p1(), penumbra[3].p1(), penumbra[3].p2()),
    Plane(penumbra[3].p1(), penumbra[0].p1(), penumbra[0].p2()),
  };

  Plane fronts[4] = {
    Plane(penumbra[0].p1(), penumbra[1].p1(), penumbra[1].p3()),
    Plane(penumbra[1].p1(), penumbra[2].p1(), penumbra[2].p3()),
    Plane(penumbra[2].p1(), penumbra[3].p1(), penumbra[3].p3()),
    Plane(penumbra[3].p1(), penumbra[0].p1(), penumbra[0].p3()),
  };

  for (int i = 0; i < 4; ++i) {
    int j = (i + 1) % 4;
    size_t base = buffer.vertex_count();

    buffer.AddTriangle(
        { penumbra[i].p1(), { sides[i], backs[i], fronts[i], sides[j] } },
        { penumbra[i].p2(), { sides[i], backs[i], fronts[i], sides[j] } },
        { penumbra[i].p3(), { sides[i], backs[i], fronts[i], sides[j] } });

    buffer.AddTriangle(
        { penumbra[j].p1(), { sides[i], backs[i], fronts[i], sides[j] } },
        { penumbra[j].p3(), { sides[i], backs[i], fronts[i], sides[j] } },
        { penumbra[j].p2(), { sides[i], backs[i], fronts[i], sides[j] } });

    // Front
    buffer.AddQuadIndices(base + 0, base + 2, base + 4, base + 3);

    // Back
    buffer.AddQuadIndices(base + 0, base + 3, base + 5, base + 1);
  }

  // buffer.AddQuad({ penumbra[0].p1(), { Plane(), Plane(), Plane(), Plane() } },
  //                { penumbra[1].p1(), { Plane(), Plane(), Plane(), Plane() } },
  //                { penumbra[2].p1(), { Plane(), Plane(), Plane(), Plane() } },
  //                { penumbra[3].p1(), { Plane(), Plane(), Plane(), Plane() } });

  return buffer;
}

}  // namespace vfx
