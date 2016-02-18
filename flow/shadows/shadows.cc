// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/shadows/shadows.h"

#include <memory>
#include <math.h>

#include "base/logging.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/utils/SkMatrix44.h"
#include "ui/gfx/geometry/quad_f.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_implementation.h"

namespace flow {
namespace {

struct Point {
  GLfloat data[3];

  void Normalize() {
    GLfloat norm = sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
    data[0] /= norm;
    data[1] /= norm;
    data[2] /= norm;
  }
};

struct Quad {
  Point point[4];
};

struct Index {
  GLubyte data[3];
};

struct Color {
  GLfloat data[4];
};

struct DrawBuffer {
  std::vector<Quad> quads;
  std::vector<Index> indices;

  void AddQuad(Quad quad) {
    GLubyte base = quads.size();
    quads.push_back(std::move(quad));
    indices.emplace_back(Index{{ base + 0, base + 1, base + 2 }});
    indices.emplace_back(Index{{ base + 2, base + 3, base + 0 }});
  }
};

struct Scene {
  std::vector<Quad> quads;
  std::vector<Index> indices;
  std::vector<Color> colors;

  void AddObject(Quad quad, Color color) {
    GLubyte base = quads.size() * 4;
    quads.push_back(std::move(quad));
    colors.push_back(color);
    indices.emplace_back(Index{{ base + 0, base + 1, base + 2 }});
    indices.emplace_back(Index{{ base + 2, base + 3, base + 0 }});
  }

  std::vector<GLfloat> GetArrayBuffer() {
    std::vector<GLfloat> array_buffer;
    array_buffer.reserve(quads.size() * 4 * 7);
    for (size_t i = 0; i < quads.size(); ++i) {
      const Quad& quad = quads[i];
      const Color& color = colors[i];
      for (size_t j = 0; j < 4; ++j) {
        const Point& point = quad.point[j];
        array_buffer.push_back(point.data[0]);
        array_buffer.push_back(point.data[1]);
        array_buffer.push_back(point.data[2]);
        array_buffer.push_back(color.data[0]);
        array_buffer.push_back(color.data[1]);
        array_buffer.push_back(color.data[2]);
        array_buffer.push_back(color.data[3]);
      }
    }
    return array_buffer;
  }
};

DrawBuffer GetShadowVolume(const Quad& quad) {
  DrawBuffer shadow;

  float far = 50.0f;
  Point light = {{ -2.0f, 5.0f, 5.0f }};

  Point back[4];
  for (int i = 0; i < 4; i++) {
    back[i].data[0] = (quad.point[i].data[0] - light.data[0]);
    back[i].data[1] = (quad.point[i].data[1] - light.data[1]);
    back[i].data[2] = (quad.point[i].data[2] - light.data[2]);
    back[i].Normalize();
    back[i].data[0] *= far;
    back[i].data[1] *= far;
    back[i].data[2] *= far;
    back[i].data[0] += light.data[0];
    back[i].data[1] += light.data[1];
    back[i].data[2] += light.data[2];
  }

  // Back cap.
  shadow.AddQuad(Quad{{ back[3], back[2], back[1], back[0] }});

  // Front cap.
  shadow.AddQuad(quad);

  // Sides
  // shadow.AddQuad(Quad{{ quad.point[0], quad.point[1], back[1], back[0] }});

  // shadow.AddQuad(Quad{{ back[0], back[1], quad.point[1], quad.point[0] }});


  shadow.AddQuad(Quad{{ quad.point[0], back[0], back[1], quad.point[1] }});
  shadow.AddQuad(Quad{{ quad.point[1], back[1], back[2], quad.point[2] }});
  shadow.AddQuad(Quad{{ quad.point[2], back[2], back[3], quad.point[3] }});
  shadow.AddQuad(Quad{{ quad.point[3], back[3], back[0], quad.point[0] }});

  return shadow;
}

Scene CreateDemoScene() {
  Scene scene;
  scene.AddObject(Quad{{
    {{  1, -1,  0 }},
    {{  1,  1,  0 }},
    {{ -1,  1,  0 }},
    {{ -1, -1,  0 }},
  }},
  Color{
    { 0, 1, 0, 1 },
  });
  scene.AddObject(Quad{{
    {{  2, -2, -1 }},
    {{  2,  2, -1 }},
    {{ -2,  2, -1 }},
    {{ -2, -2, -1 }},
  }},
  Color{
    { 0, 1, 1, 1 }
  });
  scene.AddObject(Quad{{
    {{  10, -10, -2 }},
    {{  10,  10, -2 }},
    {{ -10,  10, -2 }},
    {{ -10, -10, -2 }},
  }},
  Color{
    { 1, 1, 1, 1 }
  });
  return scene;
}

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
    "varying lowp vec4 v_color;                          \n"
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

static Scene g_demo_scene;
static std::vector<GLfloat> g_array_buffer;
static DrawBuffer g_shadow_geometry;

static GLuint g_program;
static GLint g_mvp_matrix;
static GLint g_position_location;
static GLint g_color_location;

static GLuint g_vertex_buffer;
static GLuint g_index_buffer;

static GLuint g_shadow_vertex_buffer;
static GLuint g_shadow_index_buffer;

static GLuint g_shadow_mask_vertex_buffer;
static GLuint g_shadow_mask_index_buffer;

static GLfloat g_identity_matrix[] = {
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f,
};

static GLfloat g_shadow_mask_verticies[] = {
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
   1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
   1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

static GLubyte g_shadow_mask_indicies[] = {
  0, 1, 2,
  2, 3, 0,
};

} // namespace

void DrawShadowTest(int width, int height) {
  if (!g_program) {
    g_demo_scene = CreateDemoScene();
    g_array_buffer = g_demo_scene.GetArrayBuffer();
    g_shadow_geometry = GetShadowVolume(g_demo_scene.quads[0]);

    g_program = LoadProgram(vertex_shader_source, fragment_shader_source);
    glUseProgram(g_program);
    g_mvp_matrix = glGetUniformLocation(g_program, "u_mvpMatrix");
    g_position_location = glGetAttribLocation(g_program, "a_position");
    g_color_location = glGetAttribLocation(g_program, "a_color");
    glEnableVertexAttribArray(g_position_location);

    glGenBuffersARB(1, &g_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, g_array_buffer.size() * sizeof(GLfloat), g_array_buffer.data(), GL_STATIC_DRAW);
 
    glGenBuffersARB(1, &g_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_demo_scene.indices.size() * sizeof(Index), g_demo_scene.indices.data(), GL_STATIC_DRAW);

    glGenBuffersARB(1, &g_shadow_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_shadow_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, g_shadow_geometry.quads.size() * sizeof(Quad), g_shadow_geometry.quads.data(), GL_STATIC_DRAW);

    glGenBuffersARB(1, &g_shadow_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_shadow_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_shadow_geometry.indices.size() * sizeof(Index), g_shadow_geometry.indices.data(), GL_STATIC_DRAW);

    glGenBuffersARB(1, &g_shadow_mask_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_shadow_mask_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_shadow_mask_verticies), g_shadow_mask_verticies, GL_STATIC_DRAW);

    glGenBuffersARB(1, &g_shadow_mask_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_shadow_mask_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_shadow_mask_indicies), g_shadow_mask_indicies, GL_STATIC_DRAW);
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  float aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);
  SkMatrix44 mvp = CreatePerspective(60.0f, aspect, 1.0f, 20.0f );
  mvp.preTranslate(0, 0, -2);

  glUniformMatrix4fv(g_mvp_matrix, 1, 0, reinterpret_cast<GLfloat*>(&mvp));

  // Scene geometry.

  glBindBuffer(GL_ARRAY_BUFFER, g_vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_index_buffer);
  glEnableVertexAttribArray(g_color_location);

  glVertexAttribPointer(g_position_location, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
  glVertexAttribPointer(g_color_location, 4, GL_FLOAT, GL_FALSE,  7 * sizeof(GLfloat), (GLvoid*) (sizeof(float) * 3));
 
  glDrawElements(GL_TRIANGLES, g_demo_scene.indices.size() * sizeof(Index), GL_UNSIGNED_BYTE, 0);

  // Shadows volumes.

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthMask(GL_FALSE);
  glEnable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(0.0f, 100.0f);

  glBindBuffer(GL_ARRAY_BUFFER, g_shadow_vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_shadow_index_buffer);
  glDisableVertexAttribArray(g_color_location);
  glVertexAttribPointer(g_position_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

  glCullFace(GL_FRONT);
  glStencilFunc(GL_ALWAYS, 0x0, 0xff);
  glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

  glDrawElements(GL_TRIANGLES, g_shadow_geometry.indices.size() * sizeof(Index), GL_UNSIGNED_BYTE, 0);

  glCullFace(GL_BACK);
  glStencilFunc(GL_ALWAYS, 0x0, 0xff);
  glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

  glDrawElements(GL_TRIANGLES, g_shadow_geometry.indices.size() * sizeof(Index), GL_UNSIGNED_BYTE, 0);

  // Shadows.

  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_CULL_FACE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);

  glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

  glUniformMatrix4fv(g_mvp_matrix, 1, 0, g_identity_matrix);
  glDisable(GL_DEPTH_TEST);

  glBindBuffer(GL_ARRAY_BUFFER, g_shadow_mask_vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_shadow_mask_index_buffer);
  glEnableVertexAttribArray(g_color_location);

  glVertexAttribPointer(g_position_location, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
  glVertexAttribPointer(g_color_location, 4, GL_FLOAT, GL_FALSE,  7 * sizeof(GLfloat), (GLvoid*) (sizeof(float) * 3));

  glDrawElements(GL_TRIANGLES, sizeof(g_shadow_mask_indicies), GL_UNSIGNED_BYTE, 0);

  glEnable(GL_DEPTH_TEST);

  glDisable(GL_STENCIL_TEST);
}

}  // namespace flow
