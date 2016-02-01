// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_QUADS_RENDER_PASS_QUAD_H_
#define FLOW_QUADS_RENDER_PASS_QUAD_H_

#include <memory>
#include <vector>

#include "flow/quads/quad.h"

namespace flow {

class EphemeralQuad : public Quad {
 public:
  EphemeralQuad();
  ~EphemeralQuad() override;

  void Rasterize(QuadRasterizer* rasterizer) const override;

 private:
  std::vector<std::unique_ptr<Quad>> children_;

  DISALLOW_COPY_AND_ASSIGN(EphemeralQuad);
};

}  // namespace flow

#endif  // FLOW_QUADS_RENDER_PASS_QUAD_H_
