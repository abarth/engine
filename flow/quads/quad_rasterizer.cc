// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/quad_rasterizer.h"

#include "base/logging.h"
#include "flow/quads/quad.h"

namespace flow {

QuadRasterizer::QuadRasterizer(GrContext* gr_context,
                               GrRenderTarget* render_target)
  : gr_context_(gr_context), render_target_(render_target) {
}

QuadRasterizer::~QuadRasterizer() {
}

void QuadRasterizer::Rasterize(const std::vector<std::unique_ptr<Quad>>& quads,
                               const SkPoint& offset) {
  for (auto& quad : quads)
    quad->Rasterize(this, offset);
}

QuadRasterizer::CanvasScope QuadRasterizer::GetCanvasScope() {
  return CanvasScope(skia::AdoptRef(
      SkSurface::NewRenderTargetDirect(render_target_)));
}

QuadRasterizer::DrawScope QuadRasterizer::GetDrawScope() {
  return DrawScope(skia::AdoptRef(gr_context_->drawContext(render_target_)));
}

QuadRasterizer::CanvasScope::CanvasScope(skia::RefPtr<SkSurface> surface)
  : surface_(surface) {
  DCHECK(surface_);
  canvas_ = surface_->getCanvas();
}

QuadRasterizer::CanvasScope::CanvasScope(CanvasScope&& other) {
  surface_ = std::move(other.surface_);
  canvas_ = other.canvas_;
  other.canvas_ = nullptr;
}

QuadRasterizer::CanvasScope::~CanvasScope() {
  if (canvas_)
    canvas_->flush();
}

QuadRasterizer::DrawScope::DrawScope(skia::RefPtr<GrDrawContext> context)
  : context_(context) {
}

QuadRasterizer::DrawScope::DrawScope(DrawScope&& other) {
  context_ = std::move(other.context_);
}

QuadRasterizer::DrawScope::~DrawScope() {
}

}  // namespace flow
