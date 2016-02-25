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
  geometry_.BufferData(GL_STATIC_DRAW);

  shadow_ = scene_.BuildShadowVolume();
  shadow_.BufferData(GL_STATIC_DRAW);

  Color black(0.0f, 0.0f, 0.0f, 1.0f);
  shadow_mask_.AddQuad({ Point(-1.0f, -1.0f, 0.0f), black},
                       { Point(-1.0f,  1.0f, 0.0f), black},
                       { Point( 1.0f,  1.0f, 0.0f), black},
                       { Point( 1.0f, -1.0f, 0.0f), black});
  shadow_mask_.BufferData(GL_STATIC_DRAW);
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

  geometry_.Bind();
  glVertexAttribPointer(quad_program_->position(), 3, GL_FLOAT, GL_FALSE, sizeof(ShadowScene::Vertex), 0);
  glVertexAttribPointer(quad_program_->color(), 4, GL_FLOAT, GL_FALSE, sizeof(ShadowScene::Vertex), (GLvoid*) (sizeof(GLfloat) * 3));
  geometry_.Draw();

  // Shadows volumes

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthMask(GL_FALSE);
  glEnable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(0.0f, 100.0f);

  glDisableVertexAttribArray(quad_program_->color());
  shadow_.Bind();
  glVertexAttribPointer(quad_program_->position(), 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);

  glCullFace(GL_FRONT);
  glStencilFunc(GL_ALWAYS, 0x0, 0xff);
  glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

  shadow_.Draw();

  glCullFace(GL_BACK);
  glStencilFunc(GL_ALWAYS, 0x0, 0xff);
  glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

  shadow_.Draw();

  // Shadows.

  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_CULL_FACE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_FALSE);

  glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

  Matrix identity;

  glUniformMatrix4fv(quad_program_->mvp_matrix(), 1, GL_FALSE, identity.data());
  glDisable(GL_DEPTH_TEST);

  shadow_mask_.Bind();

  glEnableVertexAttribArray(quad_program_->color());

  glVertexAttribPointer(quad_program_->position(), 3, GL_FLOAT, GL_FALSE, sizeof(ShadowScene::Vertex), 0);
  glVertexAttribPointer(quad_program_->color(), 4, GL_FLOAT, GL_FALSE,  sizeof(ShadowScene::Vertex), (GLvoid*) (sizeof(float) * 3));

  shadow_mask_.Draw();

  glEnable(GL_DEPTH_TEST);

  glDisable(GL_STENCIL_TEST);
  glDepthMask(GL_TRUE);

}

}  // namespace vfx
