// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_QUADS_QUAD_STATE_H_
#define FLOW_QUADS_QUAD_STATE_H_

#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/include/core/SkXfermode.h"

class GrPaint;

namespace flow {

class QuadState {
 public:
  QuadState();
  ~QuadState();

  const SkMatrix& transform() const { return transform_; }
  void set_transform(SkMatrix transform) { transform_ = std::move(transform); }

  const SkRect& target_rect() const { return target_rect_; }
  void set_target_rect(SkRect rect) { target_rect_ = std::move(rect); }

  const GrClip& clip() const { return clip_; }

  bool has_alpha() { return alpha_ != SK_AlphaOPAQUE; }
  int alpha() const { return alpha_; }
  void set_alpha(int alpha) { alpha_ = alpha; }

  bool has_color_filter() const { return has_color_filter_; }
  SkColor color() const { return color_; }
  SkXfermode::Mode transfer_mode() const { return transfer_mode_; }
  void SetColorFilter(SkColor color, SkXfermode::Mode tranfer_mode);
  void ClearColorFilter();

  void AddFragmentProcessors(GrPaint* paint);

 private:
  SkMatrix transform_;
  SkRect target_rect_;
  GrClip clip_;
  int alpha_;
  bool has_color_filter_;
  SkColor color_;
  SkXfermode::Mode tranfer_mode_;
};

}  // namespace flow

#endif  // FLOW_QUADS_QUAD_STATE_H_

