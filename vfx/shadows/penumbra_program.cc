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
attribute vec4 a_edge0;
attribute vec4 a_edge1;
attribute vec4 a_front0;
attribute vec4 a_front1;
attribute vec4 a_back0;
attribute vec4 a_back1;

varying vec4 v_boundary0;
varying vec4 v_boundary1;
varying vec4 v_boundary2;
varying vec4 v_boundary3;

vec4 getPlane(vec3 a, vec3 b, vec3 c) {
  vec3 v = b - a;
  vec3 w = c - a;
  vec3 normal = cross(v, w);
  return vec4(normal, -dot(normal, a));
}

void main() {
  gl_Position = u_transform * a_position;

  vec4 h_edge0  = u_transform * a_edge0;
  vec4 h_edge1  = u_transform * a_edge1;
  vec4 h_front0 = u_transform * a_front0;
  vec4 h_front1 = u_transform * a_front1;
  vec4 h_back0  = u_transform * a_back0;
  vec4 h_back1  = u_transform * a_back1;

  vec3 edge0  = (h_edge0).xyz  / h_edge0.w;
  vec3 edge1  = (h_edge1).xyz  / h_edge1.w;
  vec3 front0 = (h_front0).xyz / h_front0.w;
  vec3 front1 = (h_front1).xyz / h_front1.w;
  vec3 back0  = (h_back0).xyz  / h_back0.w;
  vec3 back1  = (h_back1).xyz  / h_back1.w;

  v_boundary0 = getPlane(edge0, front0, back0);
  v_boundary1 = getPlane(edge1, back1, front1);
  v_boundary2 = getPlane(edge0, edge1, front1);
  v_boundary3 = getPlane(edge1, edge0, back0);
}
)GLSL";

const char kFragmentShaderSource[] = R"GLSL(
#extension GL_EXT_shader_framebuffer_fetch : require
precision highp float;

varying highp vec4 v_boundary0;
varying highp vec4 v_boundary1;
varying highp vec4 v_boundary2;
varying highp vec4 v_boundary3;
uniform highp sampler2D u_depth;
uniform vec2 u_inverse_size;

const highp vec4 zero4 = vec4(0.0, 0.0, 0.0, 0.0);

void main(void) {
  // Convert gl_FragCoord from window coordiates to [0:1]^2 for texture lookup.
  highp vec2 depth_coord = gl_FragCoord.xy * u_inverse_size;

  // Depth is in the range [0:1].
  highp float depth = texture2D(u_depth, depth_coord).r;

  // Convert from [0:1]^3 to [-1:1]^3 (i.e., normalized device coordinates).
  highp vec4 position = vec4(depth_coord, depth, 1.0) * 2.0 - 1.0;

  highp vec4 dots = vec4(dot(position, v_boundary0),
                         dot(position, v_boundary1),
                         dot(position, v_boundary2),
                         dot(position, v_boundary3));

  if (any(greaterThan(dots, zero4)))
    discard;

  highp float d = dots.z + dots.w;
  lowp float a = dots.w / d;

  gl_FragColor = gl_LastFragData[0] * a;
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
    a_position_(glGetAttribLocation(program_.id(), "a_position")),
    a_edge0_(glGetAttribLocation(program_.id(), "a_edge0")),
    a_back0_(glGetAttribLocation(program_.id(), "a_back0")),
    a_front0_(glGetAttribLocation(program_.id(), "a_front0")),
    a_edge1_(glGetAttribLocation(program_.id(), "a_edge1")),
    a_back1_(glGetAttribLocation(program_.id(), "a_back1")),
    a_front1_(glGetAttribLocation(program_.id(), "a_front1")) {
  glEnableVertexAttribArray(a_position_);
  glEnableVertexAttribArray(a_edge0_);
  glEnableVertexAttribArray(a_edge1_);
  glEnableVertexAttribArray(a_front0_);
  glEnableVertexAttribArray(a_front1_);
  glEnableVertexAttribArray(a_back0_);
  glEnableVertexAttribArray(a_back1_);
}

PenumbraProgram::~PenumbraProgram() {
}

}  // namespace vfx
