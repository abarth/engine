// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/quad_compositor.h"

#include "base/logging.h"
#include "flow/quads/quad.h"

namespace flow {

QuadCompositor::QuadCompositor() {
}

QuadCompositor::~QuadCompositor() {
}

void QuadCompositor::Composite(const std::vector<std::unique_ptr<Quad>>& quads,
                               const SkPoint& offset) {
  for (auto& quad : quads)
    quad->Composite(this, offset);
}

}  // namespace flow
