// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_QUADS_TEXTURE_QUAD_H_
#define FLOW_QUADS_TEXTURE_QUAD_H_

#include "flow/quads/quad.h"
#include "third_party/skia/include/gpu/GrTexture.h"

namespace flow {

class TextureQuad : public Quad {
 public:
  TextureQuad();
  ~TextureQuad() override;

  void Rasterize(QuadRasterizer* rasterizer) const override;

 private:
  skia::RefPtr<GrTexture> texture_;

  DISALLOW_COPY_AND_ASSIGN(TextureQuad);
};

}  // namespace flow

#endif  // FLOW_QUADS_TEXTURE_QUAD_H_
