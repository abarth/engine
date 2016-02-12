// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_QUADS_QUAD_COMPOSITOR_H_
#define FLOW_QUADS_QUAD_COMPOSITOR_H_

#include <vector>

#include "base/macros.h"
#include "third_party/skia/include/core/SkPoint.h"

namespace flow {
class Quad;

class QuadCompositor {
 public:
  QuadCompositor();
  ~QuadCompositor();

  void Composite(const std::vector<std::unique_ptr<Quad>>& quads,
                 const SkPoint& offset);

 private:
  DISALLOW_COPY_AND_ASSIGN(QuadCompositor);
};

}  // namespace flow

#endif  // FLOW_QUADS_QUAD_COMPOSITOR_H_
