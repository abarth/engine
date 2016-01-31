// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/quads/quad_state.h"

namespace flow {

QuadState::QuadState() : alpha_(SK_AlphaOPAQUE) {
  transform_.setIdentity();
  target_rect_.setEmpty();
  ClearColorFilter();
}

QuadState::~QuadState() {
}

void QuadState::SetColorFilter(SkColor color, SkXfermode::Mode tranfer_mode) {
  has_color_filter_ = true;
  color_ = color;
  tranfer_mode_ = tranfer_mode;
}

void QuadState::ClearColorFilter() {
  has_color_filter_ = false;
  color_ = SK_ColorBLACK;
  tranfer_mode_ = SkXfermode::kClear_Mode;
}

void QuadState::AddFragmentProcessors(GrPaint* paint) {
  if (has_color_filter()) {
    auto processor = skia::AdoptRef(GrConstColorProcessor::Create(
        SkColorToUnpremulGrColor(color_),
        GrConstColorProcessor::kIgnore_InputMode));
    auto filter = skia::AdoptRef(
        GrXfermodeFragmentProcessor::CreateFromDstProcessor(processor.get(),
                                                            transfer_mode_);
    paint->addColorFragmentProcessor(filter.get());
  }

  if (has_alpha()) {
    auto blend = skia::AdoptRef(GrConstColorProcessor::Create(
        GrColorPackRGBA(alpha_, alpha_, alpha_, alpha_),
        GrConstColorProcessor::kModulateRGBA_InputMode));
    paint->addColorFragmentProcessor(blend.get());
  }
}

}  // namespace flow
