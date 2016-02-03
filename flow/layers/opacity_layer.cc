// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/layers/opacity_layer.h"

namespace flow {

OpacityLayer::OpacityLayer() {
}

OpacityLayer::~OpacityLayer() {
}

void OpacityLayer::GatherQuads(const SkMatrix& matrix, std::vector<std::unique_ptr<Quad>>* quads) {
  std::vector<std::unique_ptr<Quad>> child_quads;
  GatherChildQuads(matrix, &child_quads);
  if (child_quads.length() == 1) {
    std::unique_ptr<Quad> quad = std::move(child_quads.back());
    quad->state().set_alpha(alpha_);
    quads->push_back(std::move(quad));
  } else {
    std::unique_ptr<EphemeralQuad> quad(new EphemeralQuad());
    SkIRect target_rect = SkIRect::MakeEmpty();
    for (auto& child_quad : child_quads)
      target_rect.join(child_quad.state().target_rect());
    quad->set_children(std::move(child_quads));
    quad->state().set_target_rect(std::move(target_rect));
    quad->state().set_alpha(alpha_);
    quads->push_back(std::move(quad));
  }
}

void OpacityLayer::Preroll(PrerollContext* context, const SkMatrix& matrix) {
  ContainerLayer::Preroll(context, matrix);
  set_paint_bounds(context->child_paint_bounds);
}

void OpacityLayer::Paint(PaintContext::ScopedFrame& frame) {
  SkPaint paint;
  paint.setColor(SkColorSetARGB(alpha_, 0, 0, 0));
  paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);

  SkCanvas& canvas = frame.canvas();
  SkAutoCanvasRestore save(&canvas, false);
  canvas.saveLayer(&paint_bounds(), &paint);
  PaintChildren(frame);
}

}  // namespace flow
