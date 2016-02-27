// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/gl/texture_program.h"

#include <memory>

#include "ui/gl/gl_bindings.h"

namespace vfx {
namespace {

const char kVertexShaderSource[] = R"GLSL(
uniform mat4 u_transform;
attribute vec4 a_position;
attribute vec4 a_color;
attribute vec2 a_tex_coord;

varying vec2 v_tex_coord;
varying vec4 v_color;
void main()
{
  gl_Position = u_transform * a_position;
  v_color = a_color;
  v_tex_coord = a_tex_coord;
}
)GLSL";


const char kFragmentShaderSource[] = R"GLSL(
varying lowp vec4 v_color;
varying lowp vec2 v_tex_coord;
uniform sampler2D texture;

void main(void) {
    gl_FragColor = v_color * texture2D(texture, v_tex_coord);
}
)GLSL";

}  // namespace

TextureProgram::TextureProgram()
  : vertex_shader_(GL_VERTEX_SHADER, kVertexShaderSource),
    fragment_shader_(GL_FRAGMENT_SHADER, kFragmentShaderSource),
    program_(&vertex_shader_, &fragment_shader_),
    transform_(glGetUniformLocation(program_.id(), "u_transform")),
    position_(glGetAttribLocation(program_.id(), "a_position")),
    color_(glGetAttribLocation(program_.id(), "a_color")),
    tex_coord_(glGetAttribLocation(program_.id(), "a_tex_coord")) {
}

TextureProgram::~TextureProgram() {
}

}  // namespace vfx
