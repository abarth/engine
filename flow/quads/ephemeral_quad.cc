// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/ephemeral_quad.h"

#include "base/logging.h"
#include "flow/quads/quad_rasterizer.h"
#include "third_party/skia/include/gpu/GrTexture.h"

namespace flow {

EphemeralQuad::EphemeralQuad() {
  rect_.setEmpty();
}

EphemeralQuad::~EphemeralQuad() {
}

void EphemeralQuad::Rasterize(QuadRasterizer* rasterizer) const {
  GrSurfaceDesc desc;
  desc.fFlags = kRenderTarget_GrSurfaceFlag;
  desc.fWidth = rect_.width();
  desc.fHeight = rect_.height();
  desc.fConfig = kSkia8888_GrPixelConfig;
  skia::RefPtr<GrTexture> texture = skia::AdoptRef(
      rasterizer->gr_context()->textureProvider()->createTexture(desc, false));
  if (!texture)
    return;
  QuadRasterizer child_rasterizer(rasterizer->gr_context(),
                                  texture->asRenderTarget());
  child_rasterizer.Rasterize(children_);

  SkMatrix identity;
  identity.setIdentity();

  GrClip clip;

  GrPaint paint;
  paint.addColorTextureProcessor(texture.get(), identity);

  QuadRasterizer::DrawScope scope = rasterizer->GetDrawScope();
  scope.context()->drawRect(clip, paint, identity, SkRect::Make(rect_));
}

}  // namespace flow
