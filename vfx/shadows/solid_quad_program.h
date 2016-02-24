// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_SHADOWS_SOLID_QUAD_PROGRAM_H_
#define VFX_SHADOWS_SOLID_QUAD_PROGRAM_H_

#include "vfx/gl/program.h"
#include "vfx/gl/shader.h"

namespace vfx {

class SolidQuadProgram {
 public:
  SolidQuadProgram();
  ~SolidQuadProgram();

  GLuint id() const { return program_.id(); }

  GLint mvp_matrix() const { return mvp_matrix_; }
  GLint position() const { return position_; }
  GLint color() const { return color_; }

 private:
  Shader vertex_shader_;
  Shader fragment_shader_;
  Program program_;

  GLint mvp_matrix_;
  GLint position_;
  GLint color_;
};

}  // namespace vfx

#endif  // VFX_SHADOWS_SOLID_QUAD_PROGRAM_H_
