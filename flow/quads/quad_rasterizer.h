// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_QUADS_QUAD_RASTERIZER_H_
#define FLOW_QUADS_QUAD_RASTERIZER_H_

#include "base/macros.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrDrawContext.h"
#include "third_party/skia/include/gpu/GrRenderTarget.h"

namespace flow {

class QuadRasterizer {
 public:
  class CanvasScope {
   public:
    ~CanvasScope();
    SkCanvas* canvas() const { return canvas_; }

   private:
    SkCanvas* canvas_;
  };

  class DrawScope {
   public:
    ~DrawScope();
    GrDrawContext* context() const { return context_; }

   private:
    GrDrawContext* context_;
  };

  explicit QuadRasterizer(GrContext* gr_context);
  ~QuadRasterizer();

  void Rasterize(const std::vector<Quad>& quads);

  CanvasScope GetCanvas();
  DrawScope GetDrawContext();

 private:
  GrContext* gr_context_;
  GrRenderTarget* render_target_;

  DISALLOW_COPY_AND_ASSIGN(QuadRasterizer);
};

}  // namespace flow

#endif  // FLOW_QUADS_QUAD_RASTERIZER_H_
