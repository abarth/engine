// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/layers/clip_rect_layer.h"

#include "mojo/services/gfx/composition/interfaces/scenes.mojom.h"

namespace flow {

ClipRectLayer::ClipRectLayer() {
}

ClipRectLayer::~ClipRectLayer() {
}

void ClipRectLayer::Preroll(PrerollContext* context, const SkMatrix& matrix) {
  PrerollChildren(context, matrix);
  if (!context->child_paint_bounds.intersect(clip_rect_))
    context->child_paint_bounds.setEmpty();
  set_paint_bounds(context->child_paint_bounds);
}

void ClipRectLayer::Paint(PaintContext& context) {
  SkAutoCanvasRestore save(&context.canvas, true);
  context.canvas.clipRect(paint_bounds());
  PaintChildren(context);
}

void ClipRectLayer::UpdateScene(const UpdateSceneContext& context) {
  uint32_t id = 12;

  auto child_node = mojo::gfx::composition::Node::New();
  child_node->content_clip = mojo::RectF::New();
  child_node->content_clip->x = clip_rect_.x();
  child_node->content_clip->y = clip_rect_.y();
  child_node->content_clip->width = clip_rect_.width();
  child_node->content_clip->height = clip_rect_.height();
  context.update->nodes.insert(id, child_node.Pass());
  context.container->child_node_ids.push_back(id);

  UpdateSceneContext child_context = context;
  child_context.container = child_node.get();
  UpdateSceneChildren(child_context);
}

}  // namespace flow
