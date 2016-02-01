// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/quad_rasterizer.h"

namespace flow {

QuadRasterizer::QuadRasterizer(GrContext* gr_context) {
}

QuadRasterizer::~QuadRasterizer() {
}

void QuadRasterizer::Rasterize(const std::vector<std::unique_ptr<Quad>>& quads) {
  for (auto& quad : quads)
    quad->Rasterize(this);
}

}  // namespace flow
