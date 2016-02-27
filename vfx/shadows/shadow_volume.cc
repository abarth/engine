// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/shadows/shadow_volume.h"

namespace vfx {

ShadowVolume::ShadowVolume() {
}

ShadowVolume::~ShadowVolume() {
}

void ShadowVolume::Init(const Quad& occluder,
                        const Sphere& light,
                        double far) {
  // Rays from the center of the light through each vertex.
  Offset rays[4] = {
    occluder.p1() - light.center(),
    occluder.p2() - light.center(),
    occluder.p3() - light.center(),
    occluder.p4() - light.center(),
  };

  // Normals for each silhouette edge relative to the light.
  Offset normals[4] = {
    rays[0].Cross(rays[1]),
    rays[1].Cross(rays[2]),
    rays[2].Cross(rays[3]),
    rays[3].Cross(rays[0]),
  };

  // For each vertex, the average of the two adjecent normals, scaled to the
  // radius of the light.
  Offset struts[4] = {
    (normals[0] + normals[3]).Normalize() * light.radius(),
    (normals[1] + normals[0]).Normalize() * light.radius(),
    (normals[2] + normals[1]).Normalize() * light.radius(),
    (normals[3] + normals[2]).Normalize() * light.radius(),
  };

  // The points on the light that cast the rays that bound the penumbra.
  Point casters[8] = {
    // Inner edges of the penumbra.
    light.center() + struts[0],
    light.center() + struts[1],
    light.center() + struts[2],
    light.center() + struts[3],

    // Outter edges of the penumbra.
    light.center() - struts[0],
    light.center() - struts[1],
    light.center() - struts[2],
    light.center() - struts[3],
  };

  // Rays that bound the penumbra.
  Offset penumbra_rays[8] = {
    // Inner rays.
    (occluder.p1() - casters[0]).Normalize().Scale(far),
    (occluder.p2() - casters[1]).Normalize().Scale(far),
    (occluder.p3() - casters[2]).Normalize().Scale(far),
    (occluder.p4() - casters[3]).Normalize().Scale(far),

    // Outer rays
    (occluder.p1() - casters[4]).Normalize().Scale(far),
    (occluder.p2() - casters[5]).Normalize().Scale(far),
    (occluder.p3() - casters[6]).Normalize().Scale(far),
    (occluder.p4() - casters[7]).Normalize().Scale(far),
  };

  Point endpoints[8] = {
    // Umbra endpoints.
    casters[0] + penumbra_rays[0],
    casters[1] + penumbra_rays[1],
    casters[2] + penumbra_rays[2],
    casters[3] + penumbra_rays[3],

    // Penumbra endpoints.
    casters[4] + penumbra_rays[4],
    casters[5] + penumbra_rays[5],
    casters[6] + penumbra_rays[6],
    casters[7] + penumbra_rays[7],
  };

  umbra_ = Cuboid(occluder, Quad(&endpoints[0]));
  penumbra_ = Cuboid(occluder, Quad(&endpoints[4]));
}

}  // namespace vfx
