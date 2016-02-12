// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/child_scene_quad.h"

#include "base/logging.h"
#include "flow/quads/quad_rasterizer.h"

namespace flow {

ChildSceneQuad::ChildSceneQuad() {
}

ChildSceneQuad::~ChildSceneQuad() {
}

void ChildSceneQuad::Composite(QuadCompositor* rasterizer,
                               const SkPoint& paint_offset) const {
}

}  // namespace flow
