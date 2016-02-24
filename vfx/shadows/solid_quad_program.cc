// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/shadows/solid_quad_program.h"

#include <memory>

#include "ui/gl/gl_bindings.h"

namespace vfx {
namespace {

const char kVertexShaderSource[] = R"GLSL(
uniform mat4 u_mvpMatrix;
attribute vec4 a_position;
attribute vec4 a_color;
varying vec4 v_color;
void main()
{
  gl_Position = u_mvpMatrix * a_position;
  v_color = a_color;
}
)GLSL";


const char kFragmentShaderSource[] = R"GLSL(
varying lowp vec4 v_color;
void main()
{
  gl_FragColor = v_color;
}
)GLSL";

}  // namespace

SolidQuadProgram::SolidQuadProgram()
  : vertex_shader_(GL_VERTEX_SHADER, kVertexShaderSource),
    fragment_shader_(GL_FRAGMENT_SHADER, kFragmentShaderSource),
    program_(&vertex_shader_, &fragment_shader_),
    mvp_matrix_(glGetUniformLocation(program_.id(), "u_mvpMatrix")),
    position_(glGetAttribLocation(program_.id(), "a_position")),
    color_(glGetAttribLocation(program_.id(), "a_color")) {
}

SolidQuadProgram::~SolidQuadProgram() {
}

}  // namespace vfx
