// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_SHADOWS_PENUMBRA_PROGRAM_H_
#define VFX_SHADOWS_PENUMBRA_PROGRAM_H_

#include "base/macros.h"
#include "ui/gl/gl_bindings.h"
#include "vfx/geometry/matrix.h"
#include "vfx/geometry/plane.h"
#include "vfx/geometry/point.h"
#include "vfx/gl/program.h"
#include "vfx/gl/shader.h"

namespace vfx {

class PenumbraProgram {
 public:
  PenumbraProgram();
  ~PenumbraProgram();

  struct Vertex {
    Point point;
    Plane boundary[4];
  };

  GLuint id() const { return program_.id(); }

  template<typename Buffer>
  void Draw(const Matrix& transform,
            GLuint depth_id,
            const Buffer& geometry) {
    const GLvoid* kPositionOffset = nullptr;
    const GLvoid* kBoundaryOffset[4] = {
      reinterpret_cast<GLvoid*>(sizeof(GLfloat) * 3),
      reinterpret_cast<GLvoid*>(sizeof(GLfloat) * 7),
      reinterpret_cast<GLvoid*>(sizeof(GLfloat) * 11),
      reinterpret_cast<GLvoid*>(sizeof(GLfloat) * 15),
    };

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_id);

    glUseProgram(program_.id());
    glUniformMatrix4fv(u_transform_, 1, GL_FALSE, transform.data());
    glUniform1i(u_depth_, 0);
    geometry.Bind();
    glVertexAttribPointer(a_position_, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), kPositionOffset);
    glVertexAttribPointer(a_boundary_[0], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), kBoundaryOffset[0]);
    glVertexAttribPointer(a_boundary_[1], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), kBoundaryOffset[1]);
    glVertexAttribPointer(a_boundary_[2], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), kBoundaryOffset[2]);
    glVertexAttribPointer(a_boundary_[3], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), kBoundaryOffset[3]);
    geometry.Draw();
  }

 private:
  Shader vertex_shader_;
  Shader fragment_shader_;
  Program program_;

  GLint u_transform_;
  GLint u_depth_;
  GLint a_position_;
  GLint a_boundary_[4];

  DISALLOW_COPY_AND_ASSIGN(PenumbraProgram);
};

}  // namespace vfx

#endif  // VFX_SHADOWS_PENUMBRA_PROGRAM_H_
