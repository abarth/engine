// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_GEOMETRY_MATRIX_H_
#define VFX_GEOMETRY_MATRIX_H_

#include <memory>

namespace vfx {

class Matrix {
 public:
  Matrix() { SetIdentity(); }
  explicit Matrix(float data[4][4]) { memcpy(data_, data, sizeof(data_)); }

  const float* data() const { return &data_[0][0]; }

  float get(size_t i, size_t j) const { return data_[i][j]; }

  void set(size_t i, size_t j, float value) {
    data_[i][j] = value;
  }

  void SetZero();
  void SetIdentity();
  void SetFrustum(float left,
                  float right,
                  float bottom,
                  float top,
                  float near_z,
                  float far_z);
  void SetPerspective(float fov_y,
                      float aspect,
                      float near_z,
                      float far_z);

 private:
  float data_[4][4];
};

}  // namespace vfx

#endif  // VFX_GEOMETRY_MATRIX_H_
