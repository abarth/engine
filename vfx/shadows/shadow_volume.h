// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_SHADOWS_SHADOW_VOLUME_H_
#define VFX_SHADOWS_SHADOW_VOLUME_H_

#include <vector>

#include "vfx/geometry/cuboid.h"
#include "vfx/geometry/quad.h"
#include "vfx/geometry/wedge.h"
#include "vfx/geometry/sphere.h"

namespace vfx {

class ShadowVolume {
 public:
  ShadowVolume();
  ~ShadowVolume();

  void Init(const Quad& occluder, const Sphere& light, double far);

  const Triangle* shadow() const { return shadow_; }

 private:
  Triangle shadow_[4];
};

}  // namespace vfx

#endif  // VFX_SHADOWS_SHADOW_VOLUME_H_
