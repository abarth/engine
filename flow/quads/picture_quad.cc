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

void PictureQuad::Rasterize(QuadRasterizer* rasterizer) const {
  DCHECK(!state().has_alpha());
  DCHECK(!state().has_color_filter());
  QuadRasterizer::CanvasScope scope = rasterizer->GetCanvas();

  GrClip::ClipType clip_type = state().clip().clipType();
  DCHECK(clip_type == GrClip::kWideOpen_ClipType
      || clip_type == GrClip::kIRect_ClipType);

  if (clip_type == GrClip::kIRect_ClipType) {
    scope.canvas()->save();
    scope.canvas()->clipRect(SkRect::Make(state().clip().irect()));
  }

  scope.canvas()->drawPicture(picture_.get(), &state().transform(), nullptr);

  if (clip_type == GrClip::kIRect_ClipType) {
    scope.canvas()->restore();
  }
}

}  // namespace flow
