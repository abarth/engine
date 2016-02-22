// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_GEOMETRY_OFFSET_H_
#define VFX_GEOMETRY_OFFSET_H_

#include <memory>

namespace vfx {

class Offset {
 public:
  Offset() { memset(data_, 0, sizeof(data_)); }
  Offset(float data[3]) { memcpy(data_, data, sizeof(data_)); }
  Offset(float dx, float dy, float dz) {
    data_[0] = dx;
    data_[1] = dy;
    data_[2] = dz;
  }

  float dx() const { return data_[0]; }
  float dy() const { return data_[1]; }
  float dz() const { return data_[2]; }

  const float* data() const { return data_; }

  void set_dx(float dx) { data_[0] = dx; }
  void set_dy(float dy) { data_[1] = dy; }
  void set_dz(float dz) { data_[2] = dz; }

  void Scale(float factor) {
    data_[0] *= factor;
    data_[1] *= factor;
    data_[2] *= factor;
  }

  float NormSquared() const;
  void Normalize();

 private:
  float data_[3];
};

}  // namespace vfx

#endif  // VFX_GEOMETRY_OFFSET_H_
