// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_QUADS_QUAD_COLLECTOR_H_
#define FLOW_QUADS_QUAD_COLLECTOR_H_

#include <memory>
#include <vector>

#include "base/macros.h"
#include "flow/quads/quad.h"
#include "third_party/skia/include/core/SkRect.h"

namespace flow {

class QuadCollector {
 public:
  QuadCollector();
  ~QuadCollector();

  SkRect bounds;
  std::vector<std::unique_ptr<Quad>> quads;

  void set_needs_compositing() {
    needs_compositing_ = true;
  }

  bool needs_compositing() const { return needs_compositing_; }

 private:
  bool needs_compositing_ = false;

  DISALLOW_COPY_AND_ASSIGN(QuadCollector);
};

}  // namespace flow

#endif  // FLOW_QUADS_QUAD_COLLECTOR_H_
