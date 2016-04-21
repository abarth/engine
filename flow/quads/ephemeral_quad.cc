// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/ephemeral_quad.h"

#include "base/logging.h"
#include "flow/quads/quad_rasterizer.h"
#include "third_party/skia/include/gpu/effects/GrConstColorProcessor.h"
#include "third_party/skia/include/gpu/effects/GrXfermodeFragmentProcessor.h"
#include "third_party/skia/include/gpu/GrTexture.h"
#include "third_party/skia/include/gpu/SkGr.h"

namespace flow {

EphemeralQuad::EphemeralQuad() {
  rect_.setEmpty();
}

EphemeralQuad::~EphemeralQuad() {
}

void EphemeralQuad::ApplyAlpha(int alpha) {
  fragment_processors_.push_back(skia::AdoptRef(GrConstColorProcessor::Create(
      GrColorPackRGBA(alpha, alpha, alpha, alpha),
      GrConstColorProcessor::kModulateRGBA_InputMode)));
}

void EphemeralQuad::Rasterize(QuadRasterizer* rasterizer,
                              const SkPoint& offset) const {
  // LOG(INFO) << "EphemeralQuad::Rasterize rect: ("
  //           << rect_.x() << ", " << rect_.y()
  //           << ", " << rect_.width() << ", " << rect_.height() << ") offset: ("
  //           << offset.x() << ", " << offset.y() << ")";
  GrSurfaceDesc desc;
  desc.fFlags = kRenderTarget_GrSurfaceFlag;
  desc.fWidth = rect_.width();
  desc.fHeight = rect_.height();
  desc.fConfig = kSkia8888_GrPixelConfig;
  skia::RefPtr<GrTexture> texture = skia::AdoptRef(
      rasterizer->gr_context()->textureProvider()->createTexture(desc, SkBudgeted::kNo));
  if (!texture)
    return;
  QuadRasterizer child_rasterizer(rasterizer->gr_context(),
                                  texture->asRenderTarget());
  child_rasterizer.GetDrawScope().context()->clear(nullptr, SK_ColorTRANSPARENT, true);
  SkPoint child_offset = SkPoint::Make(-rect_.x(), -rect_.y());
  child_rasterizer.Rasterize(children_, child_offset);
  texture->flushWrites();

  GrPaint paint;
  paint.addColorTextureProcessor(texture.get(), SkMatrix::I());

  for (auto& processor : fragment_processors_)
    paint.addColorFragmentProcessor(processor.get());

  QuadRasterizer::DrawScope scope = rasterizer->GetDrawScope();
  scope.context()->fillRectToRect(GrClip::WideOpen(), paint, SkMatrix::I(),
                            rect_.makeOffset(offset.x(), offset.y()), SkRect::MakeWH(1.0, 1.0));
}

}  // namespace flow
