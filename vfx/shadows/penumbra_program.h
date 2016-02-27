// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_SHADOWS_PENUMBRA_PROGRAM_H_
#define VFX_SHADOWS_PENUMBRA_PROGRAM_H_

#include "base/macros.h"
#include "vfx/geometry/matrix.h"
#include "vfx/geometry/point.h"
#include "vfx/gl/program.h"
#include "vfx/gl/shader.h"

namespace vfx {

class PenumbraProgram {
 public:
  PenumbraProgram();
  ~PenumbraProgram();

  GLuint id() const { return program_.id(); }

 private:
  Shader vertex_shader_;
  Shader fragment_shader_;
  Program program_;

  GLint u_transform_;
  GLint u_depth_;
  GLint a_position_;
  GLint a_boundary0_;
  GLint a_boundary1_;
  GLint a_boundary2_;
  GLint a_boundary3_;

  DISALLOW_COPY_AND_ASSIGN(PenumbraProgram);
};

}  // namespace vfx

#endif  // VFX_SHADOWS_PENUMBRA_PROGRAM_H_
