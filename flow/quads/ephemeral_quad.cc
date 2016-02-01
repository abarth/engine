// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/render_pass_quad.h"

namespace flow {

EphemeralQuad::EphemeralQuad() {
}

EphemeralQuad::~EphemeralQuad() {
}

void EphemeralQuad::Rasterize(QuadRasterizer* rasterizer) const {
  GrSurfaceDesc desc;
  desc.fFlags = kRenderTarget_GrSurfaceFlag;
  desc.fWidth = state().target_rect().width();
  desc.fHeight = state().target_rect().height();
  desc.fConfig = kSkia8888_GrPixelConfig;
  skia::RefPtr<GrTexture> texture = skia::AdoptPtr(
      rasterizer->context()->textureProvider()->createTexture(desc, false));
  if (!texture)
    return;
  rasterizer->PushRenderTarget(texture->asRenderTarget());
  rasterizer->Rasterize(children_);
  rasterizer->PopRenderTarget();

  SkMatrix identity;
  identity.setIdentity();

  GrPaint paint;
  paint.addColorTextureProcessor(texture.get(), identity);
  state().AddFragmentProcessors(&paint);

  QuadRasterizer::DrawScope scope = rasterizer->GetDrawContext();
  scope.context()->drawRect(state().clip(), paint, identity,
                            state().target_rect());
}

}  // namespace flow
