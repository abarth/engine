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

attribute vec3 a_position;
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
  gl_Position = u_transform * vec4(a_position, 1);
  v_boundary0 = a_boundary0;
  v_boundary1 = a_boundary1;
  v_boundary2 = a_boundary2;
  v_boundary3 = a_boundary3;
}
)GLSL";


const char kFragmentShaderSource[] = R"GLSL(
#extension GL_EXT_shader_framebuffer_fetch : require
precision lowp float;

varying lowp vec4 v_boundary0;
varying lowp vec4 v_boundary1;
varying lowp vec4 v_boundary2;
varying lowp vec4 v_boundary3;
uniform highp sampler2D u_depth;
uniform vec2 u_inverse_size;

const lowp vec4 zero4 = vec4(0.0, 0.0, 0.0, 0.0);

void main(void) {
  vec2 depth_coord = gl_FragCoord.xy * u_inverse_size;
  highp float depth = texture2D(u_depth, depth_coord).r;
  // lowp float depth = gl_LastFragData[0].r;
  if (gl_FragCoord.z >= depth)
    discard;
  // lowp vec4 position = vec4(gl_FragCoord.xy, depth, 1.0);
  // if (dot(position, v_boundary2) <= 0.0)
  //   discard;
  // lowp vec4 dots = vec4(dot(position, v_boundary0),
  //                       dot(position, v_boundary1),
  //                       dot(position, v_boundary2),
  //                       dot(position, v_boundary3));
  // if (any(lessThan(dots, zero4))) {
  //   discard;
  // }
  gl_FragColor = vec4(0.5, 0.5, 0.0, 1.0);
  // gl_FragColor = vec4(depth, 0.0, 0.0, 1.0);
  // gl_FragColor = texture2D(u_depth, depth_coord);
}
)GLSL";

}  // namespace

PenumbraProgram::PenumbraProgram()
  : vertex_shader_(GL_VERTEX_SHADER, kVertexShaderSource),
    fragment_shader_(GL_FRAGMENT_SHADER, kFragmentShaderSource),
    program_(&vertex_shader_, &fragment_shader_),
    u_transform_(glGetUniformLocation(program_.id(), "u_transform")),
    u_depth_(glGetUniformLocation(program_.id(), "u_depth")),
    u_inverse_size_(glGetUniformLocation(program_.id(), "u_inverse_size")),
    a_position_(glGetAttribLocation(program_.id(), "a_position")) {
  a_boundary_[0] = glGetAttribLocation(program_.id(), "a_boundary0");
  a_boundary_[1] = glGetAttribLocation(program_.id(), "a_boundary1");
  a_boundary_[2] = glGetAttribLocation(program_.id(), "a_boundary2");
  a_boundary_[3] = glGetAttribLocation(program_.id(), "a_boundary3");
  glEnableVertexAttribArray(a_position_);
  glEnableVertexAttribArray(a_boundary_[0]);
  glEnableVertexAttribArray(a_boundary_[1]);
  glEnableVertexAttribArray(a_boundary_[2]);
  glEnableVertexAttribArray(a_boundary_[3]);
}

PenumbraProgram::~PenumbraProgram() {
}

}  // namespace vfx
