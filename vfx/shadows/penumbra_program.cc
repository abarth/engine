// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/shadows/penumbra_program.h"

#include <memory>

#include "ui/gl/gl_bindings.h"

namespace vfx {
namespace {

const char kVertexShaderSource[] = R"GLSL(
uniform mat4 u_transform;

attribute vec4 a_position;
attribute vec4 a_boundary0;
attribute vec4 a_boundary1;
attribute vec4 a_boundary2;
attribute vec4 a_boundary3;

varying vec4 v_boundary0;
varying vec4 v_boundary1;
varying vec4 v_boundary2;
varying vec4 v_boundary3;

void main()
{
  gl_Position = u_transform * a_position;
  v_boundary0 = a_boundary0;
  v_boundary1 = a_boundary1;
  v_boundary2 = a_boundary2;
  v_boundary3 = a_boundary3;
}
)GLSL";


const char kFragmentShaderSource[] = R"GLSL(
precision lowp float;

varying lowp vec4 v_boundary0;
varying lowp vec4 v_boundary1;
varying lowp vec4 v_boundary2;
varying lowp vec4 v_boundary3;
uniform sampler2D u_depth;

const lowp vec4 zero4 = vec4(0.0, 0.0, 0.0, 0.0);

void main(void) {
  lowp float depth = texture2D(u_depth, gl_FragCoord.xy).z;
  lowp vec4 position = vec4(gl_FragCoord.xy, depth, 1.0);
  lowp vec4 dots = vec4(dot(position, v_boundary0),
                        dot(position, v_boundary1),
                        dot(position, v_boundary2),
                        dot(position, v_boundary3));
  if (any(lessThan(dots, zero4))) {
    discard;
  }
  gl_FragColor = vec4(0.5, 0.5, 0.0, 1.0);
}
)GLSL";

}  // namespace

PenumbraProgram::PenumbraProgram()
  : vertex_shader_(GL_VERTEX_SHADER, kVertexShaderSource),
    fragment_shader_(GL_FRAGMENT_SHADER, kFragmentShaderSource),
    program_(&vertex_shader_, &fragment_shader_),
    u_transform_(glGetUniformLocation(program_.id(), "u_transform")),
    u_depth_(glGetUniformLocation(program_.id(), "u_depth")),
    a_position_(glGetAttribLocation(program_.id(), "a_position")),
    a_boundary0_(glGetAttribLocation(program_.id(), "a_boundary0")),
    a_boundary1_(glGetAttribLocation(program_.id(), "a_boundary1")),
    a_boundary2_(glGetAttribLocation(program_.id(), "a_boundary2")),
    a_boundary3_(glGetAttribLocation(program_.id(), "a_boundary3")) {
  glEnableVertexAttribArray(a_position_);
  glEnableVertexAttribArray(a_boundary0_);
  glEnableVertexAttribArray(a_boundary1_);
  glEnableVertexAttribArray(a_boundary2_);
  glEnableVertexAttribArray(a_boundary3_);
}

PenumbraProgram::~PenumbraProgram() {
}

}  // namespace vfx
