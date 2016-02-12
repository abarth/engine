// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/quad.h"

namespace flow {

Quad::Quad() {
}

Quad::~Quad() {
}

void Quad::Rasterize(QuadRasterizer* rasterizer, const SkPoint& offset) const {
}

void Quad::Composite(QuadCompositor* compositor, const SkPoint& offset) const {
}

}  // namespace flow
