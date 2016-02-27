// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_GL_TEXTURE_PROGRAM_H_
#define VFX_GL_TEXTURE_PROGRAM_H_

#include "base/macros.h"
#include "vfx/gl/program.h"
#include "vfx/gl/shader.h"

namespace vfx {

class TextureProgram {
 public:
  TextureProgram();
  ~TextureProgram();

  GLuint id() const { return program_.id(); }

  GLint transform() const { return transform_; }
  GLint position() const { return position_; }
  GLint color() const { return color_; }
  GLint tex_coord() const { return tex_coord_; }

 private:
  Shader vertex_shader_;
  Shader fragment_shader_;
  Program program_;

  GLint transform_;
  GLint position_;
  GLint color_;
  GLint tex_coord_;

  DISALLOW_COPY_AND_ASSIGN(TextureProgram);
};

}  // namespace vfx

#endif  // VFX_GL_TEXTURE_PROGRAM_H_
