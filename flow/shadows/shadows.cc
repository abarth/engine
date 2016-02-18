// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/shadows/shadows.h"

#include <math.h>

#include "base/logging.h"
#include "third_party/skia/include/utils/SkMatrix44.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_implementation.h"

namespace flow {
namespace {

SkMatrix44 CreateFrustum(float left,
                         float right,
                         float bottom,
                         float top,
                         float near_z,
                         float far_z) {
  float delta_x = right - left;
  float delta_y = top - bottom;
  float delta_z = far_z - near_z;

  if ((near_z <= 0.0f) ||
      (far_z <= 0.0f) ||
      (delta_z <= 0.0f) ||
      (delta_y <= 0.0f) ||
      (delta_y <= 0.0f))
    return SkMatrix44();

  SkMatrix44 frust;
  frust.set(0, 0, 2.0f * near_z / delta_x);
  frust.set(0, 1, 0.0f);
  frust.set(0, 2, 0.0f);
  frust.set(0, 3, 0.0f);

  frust.set(1, 1, 2.0f * near_z / delta_y);
  frust.set(1, 0, 0.0f);
  frust.set(1, 2, 0.0f);
  frust.set(1, 3, 0.0f);

  frust.set(2, 0, (right + left) / delta_x);
  frust.set(2, 1, (top + bottom) / delta_y);
  frust.set(2, 2, -(near_z + far_z) / delta_z);
  frust.set(2, 3, -1.0f);

  frust.set(3, 2, -2.0f * near_z * far_z / delta_z);
  frust.set(3, 0, 0.0f);
  frust.set(3, 1, 0.0f);
  frust.set(3, 3, 0.0f);
  return frust;
}

SkMatrix44 CreatePerspective(float fov_y, float aspect, float near_z, float far_z) {
  float frustum_h = tanf(fov_y / 360.0f * M_PI) * near_z;
  float frustum_w = frustum_h * aspect;
  return CreateFrustum(-frustum_w, frustum_w, -frustum_h, frustum_h, near_z, far_z);
}

const char vertex_shader_source[] =
    "uniform mat4 u_mvpMatrix;                                       \n"
    "attribute vec4 a_position;                                      \n"
    "attribute vec4 a_color;                                         \n"
    "varying vec4 v_color;                                           \n"
    "void main()                                                     \n"
    "{                                                               \n"
    "   gl_Position = u_mvpMatrix * a_position;                      \n"
    "   v_color = a_color;                                           \n"
    "}                                                               \n";


const char fragment_shader_source[] =
    "varying vec4 v_color;                               \n"
    "void main()                                         \n"
    "{                                                   \n"
    "   gl_FragColor = v_color;                          \n"
    "}                                                   \n";

GLuint LoadShader(GLenum type, const char* shader_source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &shader_source, NULL);
  glCompileShader(shader);

  GLint compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

  if (!compiled) {
    GLsizei expected_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &expected_length);
    std::string log;
    log.resize(expected_length);  // Includes null terminator.
    GLsizei actual_length = 0;
    glGetShaderInfoLog(shader, expected_length, &actual_length, &log[0]);
    log.resize(actual_length);  // Excludes null terminator.
    LOG(FATAL) << "Compilation of shader failed: " << log;
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

GLuint LoadProgram(const char* vertex_shader_source,
                   const char* fragment_shader_source) {
  GLuint vertex_shader = LoadShader(GL_VERTEX_SHADER,
                                    vertex_shader_source);
  if (!vertex_shader)
    return 0;

  GLuint fragment_shader = LoadShader(GL_FRAGMENT_SHADER,
                                      fragment_shader_source);
  if (!fragment_shader) {
    glDeleteShader(vertex_shader);
    return 0;
  }

  GLuint program_object = glCreateProgram();
  glAttachShader(program_object, vertex_shader);
  glAttachShader(program_object, fragment_shader);
  glLinkProgram(program_object);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  GLint linked = 0;
  glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
  if (!linked) {
    GLsizei expected_length = 0;
    glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &expected_length);
    std::string log;
    log.resize(expected_length);  // Includes null terminator.
    GLsizei actual_length = 0;
    glGetProgramInfoLog(program_object, expected_length, &actual_length,
                        &log[0]);
    log.resize(actual_length);  // Excludes null terminator.
    LOG(FATAL) << "Linking program failed: " << log;
    glDeleteProgram(program_object);
    return 0;
  }

  return program_object;
}

struct Vertex {
  float Position[3];
  float Color[4];
};
 
const Vertex g_vertices[] = {
  {{  1, -1,  0 }, { 0, 1, 0, 1 }},
  {{  1,  1,  0 }, { 0, 1, 0, 1 }},
  {{ -1,  1,  0 }, { 0, 1, 0, 1 }},
  {{ -1, -1,  0 }, { 0, 1, 0, 1 }},

  {{  2, -2,  -1 }, { 0, 1, 1, 1 }},
  {{  2,  2,  -1 }, { 0, 1, 1, 1 }},
  {{ -2,  2,  -1 }, { 0, 1, 1, 1 }},
  {{ -2, -2,  -1 }, { 0, 1, 1, 1 }},
};
 
const GLubyte g_indices[] = {
  0, 1, 2,
  2, 3, 0,

  4, 5, 6,
  6, 7, 4,
};

static GLuint g_program;
static GLint g_mvp_matrix;
static GLint g_position_location;
static GLint g_color_location;

} // namespace

void DrawShadowTest(int width, int height) {
  if (!g_program) {
    g_program = LoadProgram(vertex_shader_source, fragment_shader_source);
    glUseProgram(g_program);
    g_mvp_matrix = glGetUniformLocation(g_program, "u_mvpMatrix");
    g_position_location = glGetAttribLocation(g_program, "a_position");
    g_color_location = glGetAttribLocation(g_program, "a_color");
    glEnableVertexAttribArray(g_position_location);
    glEnableVertexAttribArray(g_color_location);

    GLuint vertex_buffer = 0;
    glGenBuffersARB(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertices), g_vertices, GL_STATIC_DRAW);
 
    GLuint index_buffer = 0;
    glGenBuffersARB(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_indices), g_indices, GL_STATIC_DRAW);
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);
  SkMatrix44 mvp = CreatePerspective(60.0f, aspect, 1.0f, 20.0f );
  mvp.preTranslate(0, 0, -2);

  glUniformMatrix4fv(g_mvp_matrix, 1, 0, reinterpret_cast<GLfloat*>(&mvp));
  glVertexAttribPointer(g_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
  glVertexAttribPointer(g_color_location, 4, GL_FLOAT, GL_FALSE,  sizeof(Vertex), (GLvoid*) (sizeof(float) * 3));
 
  glDrawElements(GL_TRIANGLES, arraysize(g_indices), GL_UNSIGNED_BYTE, 0);
}

}  // namespace flow
