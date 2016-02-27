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
  color_program_.reset(new ColorProgram());

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

void ShadowRenderer::Draw(const gfx::Size size) {
  if (frame_buffer_.is_null())
    frame_buffer_ = FrameBuffer(size);

  // frame_buffer_->Bind();
  glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, size.width(), size.height());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  float aspect = static_cast<GLfloat>(size.width()) / static_cast<GLfloat>(size.height());
  Matrix transform;
  transform.SetPerspective(60.0f, aspect, 1.0f, 20.0f );
  transform.PreTranslate(Offset(0, 0, -2));

  glUniformMatrix4fv(color_program_->transform(), 1, GL_FALSE, transform.data());
  glEnableVertexAttribArray(color_program_->position());
  glEnableVertexAttribArray(color_program_->color());
  glUseProgram(color_program_->id());

  // Quads

  geometry_.Bind();
  glVertexAttribPointer(color_program_->position(), 3, GL_FLOAT, GL_FALSE, sizeof(ShadowScene::Vertex), 0);
  glVertexAttribPointer(color_program_->color(), 4, GL_FLOAT, GL_FALSE, sizeof(ShadowScene::Vertex), (GLvoid*) (sizeof(GLfloat) * 3));
  geometry_.Draw();


  // glBindFramebufferEXT(GL_FRAMEBUFFER, fbo_);

  // Shadows volumes

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthMask(GL_FALSE);
  glEnable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(0.0f, 100.0f);

  glDisableVertexAttribArray(color_program_->color());
  shadow_.Bind();
  glVertexAttribPointer(color_program_->position(), 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);

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

  glUniformMatrix4fv(color_program_->transform(), 1, GL_FALSE, identity.data());
  glDisable(GL_DEPTH_TEST);

  shadow_mask_.Bind();

  glEnableVertexAttribArray(color_program_->color());

  glVertexAttribPointer(color_program_->position(), 3, GL_FLOAT, GL_FALSE, sizeof(ShadowScene::Vertex), 0);
  glVertexAttribPointer(color_program_->color(), 4, GL_FLOAT, GL_FALSE,  sizeof(ShadowScene::Vertex), (GLvoid*) (sizeof(float) * 3));

  shadow_mask_.Draw();

  glEnable(GL_DEPTH_TEST);

  glDisable(GL_STENCIL_TEST);
  glDepthMask(GL_TRUE);

}

}  // namespace vfx
