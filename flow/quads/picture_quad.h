// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_QUADS_PICTURE_QUAD_H_
#define FLOW_QUADS_PICTURE_QUAD_H_

#include <memory>

#include "flow/quads/quad.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkPicture.h"

namespace flow {

class PictureQuad : public Quad {
 public:
  PictureQuad();
  ~PictureQuad() override;

  void Rasterize(QuadRasterizer* rasterizer,
                 const SkPoint& offset) const override;

  void set_picture(skia::RefPtr<SkPicture> picture) {
    picture_ = std::move(picture);
  }

  void set_offset(SkPoint offset) {
    offset_ = std::move(offset);
  }

 private:
  SkPoint offset_;
  skia::RefPtr<SkPicture> picture_;

  DISALLOW_COPY_AND_ASSIGN(PictureQuad);
};

}  // namespace flow

#endif  // FLOW_QUADS_PICTURE_QUAD_H_
