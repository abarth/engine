// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_QUADS_CHILD_SCENE_QUAD_H_
#define FLOW_QUADS_CHILD_SCENE_QUAD_H_

#include <memory>

#include "flow/quads/quad.h"
#include "mojo/services/gfx/composition/interfaces/scenes.mojom.h"
#include "third_party/skia/include/core/SkPoint.h"
#include "third_party/skia/include/core/SkSize.h"

namespace flow {

class ChildSceneQuad : public Quad {
 public:
  ChildSceneQuad();
  ~ChildSceneQuad() override;

  void Composite(QuadCompositor* compositor,
                 const SkPoint& offset) const override;

  void set_offset(SkPoint offset) {
    offset_ = std::move(offset);
  }

  void set_physical_size(SkISize physical_size) {
    physical_size_ = std::move(physical_size);
  }

  void set_scene_token(mojo::gfx::composition::SceneTokenPtr scene_token) {
    scene_token_ = scene_token.Pass();
  }

 private:
  SkPoint offset_;
  SkISize physical_size_;
  mojo::gfx::composition::SceneTokenPtr scene_token_;

  DISALLOW_COPY_AND_ASSIGN(ChildSceneQuad);
};

}  // namespace flow

#endif  // FLOW_QUADS_CHILD_SCENE_QUAD_H_
