// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/texture_quad.h"

#include "flow/quads/quad_rasterizer.h"
#include "third_party/skia/include/gpu/effects/GrConstColorProcessor.h"
#include "third_party/skia/include/gpu/effects/GrXfermodeFragmentProcessor.h"
#include "third_party/skia/include/gpu/GrPaint.h"
#include "third_party/skia/include/gpu/SkGr.h"

namespace flow {

TextureQuad::TextureQuad() {
}

TextureQuad::~TextureQuad() {
}

void TextureQuad::Rasterize(QuadRasterizer* rasterizer) const {
  GrPaint paint;
  paint.addColorTextureProcessor(texture_.get(), state().transform());
  state().AddFragmentProcessors(&paint);

  SkMatrix identity;
  identity.setIdentity();

  QuadRasterizer::DrawScope scope = rasterizer->GetDrawContext();
  scope.context()->drawRect(state().clip(), paint, identity,
                            SkRect::Make(state().target_rect()));
}

}  // namespace flow
