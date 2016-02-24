// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/shadows/shadow_renderer.h"

#include <memory>

#include "vfx/geometry/matrix.h"

namespace vfx {

ShadowRenderer::ShadowRenderer(ShadowScene scene)
  : scene_(std::move(scene)) {
}

ShadowRenderer::~ShadowRenderer() {
}

void ShadowRenderer::PrepareToDraw() {
  quad_program_.reset(new SolidQuadProgram());

  geometry_ = scene_.BuildGeometry();
  geometry_binding_.BufferData(geometry_, GL_STATIC_DRAW);

  shadow_volume_ = scene_.BuildShadowVolume();
}

void ShadowRenderer::Draw(int width, int height) {
  glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  float aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);
  Matrix mvp;
  mvp.SetPerspective(60.0f, aspect, 1.0f, 20.0f );
  mvp.PreTranslate(Offset(0, 0, -2));

  glUniformMatrix4fv(quad_program_->mvp_matrix(), 1, GL_FALSE, mvp.data());
  glEnableVertexAttribArray(quad_program_->position());
  glEnableVertexAttribArray(quad_program_->color());
  glUseProgram(quad_program_->id());

  // Quads

  geometry_binding_.Bind();
  glVertexAttribPointer(quad_program_->position(), 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
  glVertexAttribPointer(quad_program_->color(), 4, GL_FLOAT, GL_FALSE,  7 * sizeof(GLfloat), (GLvoid*) (sizeof(GLfloat) * 3));
  glDrawElements(GL_TRIANGLES, geometry_binding_.count(), GL_UNSIGNED_BYTE, 0);
}

}  // namespace vfx
