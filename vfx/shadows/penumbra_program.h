// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_SHADOWS_PENUMBRA_PROGRAM_H_
#define VFX_SHADOWS_PENUMBRA_PROGRAM_H_

#include "base/macros.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gl/gl_bindings.h"
#include "vfx/geometry/matrix.h"
#include "vfx/geometry/point.h"
#include "vfx/geometry/wedge.h"
#include "vfx/gl/program.h"
#include "vfx/gl/shader.h"

namespace vfx {

class PenumbraProgram {
 public:
  PenumbraProgram();
  ~PenumbraProgram();

  struct Vertex {
    Point point;
    Wedge penumbra;
  };

  GLuint id() const { return program_.id(); }

  template<typename Buffer>
  void Draw(const Matrix& transform,
            GLuint depth_id,
            const gfx::Size viewport_size,
            const Buffer& geometry) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_id);

    glUseProgram(program_.id());
    glUniformMatrix4fv(u_transform_, 1, GL_FALSE, transform.data());
    glUniform1i(u_depth_, 0);
    glUniform2f(u_inverse_size_, 1.0 / viewport_size.width(), 1.0 / viewport_size.height());
    geometry.Bind();
    glVertexAttribPointer(a_position_, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), GetOffset(0));

    glVertexAttribPointer(a_edge0_, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), GetOffset(3));
    glVertexAttribPointer(a_back0_, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), GetOffset(6));
    glVertexAttribPointer(a_front0_, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), GetOffset(9));

    glVertexAttribPointer(a_edge1_, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), GetOffset(12));
    glVertexAttribPointer(a_back1_, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), GetOffset(15));
    glVertexAttribPointer(a_front1_, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), GetOffset(18));
    geometry.Draw();
  }

 private:
  static GLvoid* GetOffset(int n) {
    return reinterpret_cast<GLvoid*>(sizeof(GLfloat) * n);
  }

  Shader vertex_shader_;
  Shader fragment_shader_;
  Program program_;

  GLint u_transform_;
  GLint u_depth_;
  GLint u_inverse_size_;
  GLint a_position_;

  GLint a_edge0_;
  GLint a_back0_;
  GLint a_front0_;

  GLint a_edge1_;
  GLint a_back1_;
  GLint a_front1_;

  DISALLOW_COPY_AND_ASSIGN(PenumbraProgram);
};

}  // namespace vfx

#endif  // VFX_SHADOWS_PENUMBRA_PROGRAM_H_
