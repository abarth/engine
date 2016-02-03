// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_QUADS_QUAD_RASTERIZER_H_
#define FLOW_QUADS_QUAD_RASTERIZER_H_

#include <vector>

#include "base/macros.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrDrawContext.h"
#include "third_party/skia/include/gpu/GrRenderTarget.h"

namespace flow {
class Quad;

class QuadRasterizer {
 public:
  class CanvasScope {
   public:
    CanvasScope(CanvasScope&& other);
    ~CanvasScope();
    SkCanvas* canvas() const { return canvas_; }

   private:
    explicit CanvasScope(skia::RefPtr<SkSurface> surface);

    friend class QuadRasterizer;

    skia::RefPtr<SkSurface> surface_;
    SkCanvas* canvas_;

    DISALLOW_COPY_AND_ASSIGN(CanvasScope);
  };

  class DrawScope {
   public:
    DrawScope(DrawScope&& other);
    ~DrawScope();
    GrDrawContext* context() const { return context_.get(); }

   private:
    explicit DrawScope(skia::RefPtr<GrDrawContext> context);

    friend class QuadRasterizer;

    skia::RefPtr<GrDrawContext> context_;

    DISALLOW_COPY_AND_ASSIGN(DrawScope);
  };

  QuadRasterizer(GrContext* gr_context, GrRenderTarget* render_target);
  ~QuadRasterizer();

  GrContext* gr_context() const { return gr_context_; }

  void Rasterize(const std::vector<std::unique_ptr<Quad>>& quads);

  CanvasScope GetCanvas();
  DrawScope GetDrawContext();

 private:
  GrContext* gr_context_;
  GrRenderTarget* render_target_;

  DISALLOW_COPY_AND_ASSIGN(QuadRasterizer);
};

}  // namespace flow

#endif  // FLOW_QUADS_QUAD_RASTERIZER_H_
