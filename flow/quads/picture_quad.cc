// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/picture_quad.h"

#include "base/logging.h"
#include "flow/quads/quad_rasterizer.h"

namespace flow {

PictureQuad::PictureQuad() {
}

PictureQuad::~PictureQuad() {
}

void PictureQuad::Rasterize(QuadRasterizer* rasterizer,
                            const SkPoint& paint_offset) const {
  // LOG(INFO) << "PictureQuad::Rasterize offset_: ("
  //           << offset_.x() << ", " << offset_.x() << ") "
  //           << "paint_offset: (" << paint_offset.x() << ", " << paint_offset.x() << ") ";

  auto scope = rasterizer->GetCanvasScope();
  SkAutoCanvasRestore save(scope.canvas(), true);
  scope.canvas()->translate(offset_.x() + paint_offset.x(),
                            offset_.y() + paint_offset.y());
  scope.canvas()->drawPicture(picture_.get());
}

}  // namespace flow
