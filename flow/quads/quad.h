// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_QUADS_QUAD_H_
#define FLOW_QUADS_QUAD_H_

#include "base/macros.h"
#include "third_party/skia/include/core/SkPoint.h"

namespace flow {
class QuadRasterizer;

class Quad {
 public:
  Quad();
  virtual ~Quad();

  virtual void Rasterize(QuadRasterizer* rasterizer,
                         const SkPoint& offset) const = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(Quad);
};

}  // namespace flow

#endif  // FLOW_QUADS_QUAD_H_
