// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/geometry/matrix.h"

#include <math.h>

namespace vfx {

static_assert(sizeof(Matrix) == sizeof(float) * 16,
              "Matrix should contain only floats");

void Matrix::SetZero() {
  memset(data_, 0, sizeof(data_));
}

void Matrix::SetIdentity() {
  memset(data_, 0, sizeof(data_));
  data_[0][0] = 1.0f;
  data_[1][1] = 1.0f;
  data_[2][2] = 1.0f;
  data_[3][3] = 1.0f;
}

void Matrix::SetFrustum(float left,
                        float right,
                        float bottom,
                        float top,
                        float near_z,
                        float far_z) {
  float delta_x = right - left;
  float delta_y = top - bottom;
  float delta_z = far_z - near_z;

  if ((near_z <= 0.0f) ||
      (far_z <= 0.0f) ||
      (delta_z <= 0.0f) ||
      (delta_y <= 0.0f) ||
      (delta_y <= 0.0f)) {
    SetIdentity();
  } else {
    data_[0][0] = 2.0f * near_z / delta_x;
    data_[0][1] = 0.0f;
    data_[0][2] = 0.0f;
    data_[0][3] = 0.0f;

    data_[1][0] = 0.0f;
    data_[1][1] = 2.0f * near_z / delta_y;
    data_[1][2] = 0.0f;
    data_[1][3] = 0.0f;

    data_[2][0] = (right + left) / delta_x;
    data_[2][1] = (top + bottom) / delta_y;
    data_[2][2] = -(near_z + far_z) / delta_z;
    data_[2][3] = -1.0f;

    data_[3][0] = 0.0f;
    data_[3][1] = 0.0f;
    data_[3][2] = -2.0f * near_z * far_z / delta_z;
    data_[3][3] = 0.0f;
  }
}

void Matrix::SetPerspective(float fov_y,
                            float aspect,
                            float near_z,
                            float far_z) {
  float dy = tanf(fov_y / 360.0f * M_PI) * near_z;
  float dx = dy * aspect;
  SetFrustum(-dx, dx, -dy, dy, near_z, far_z);
}

}  // namespace vfx
