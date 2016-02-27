// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/shadows/shadow_renderer.h"

#include <memory>

#include "vfx/geometry/matrix.h"
#include "ui/gl/scoped_binders.h"

namespace vfx {

ShadowRenderer::ShadowRenderer(ShadowScene scene)
  : scene_(std::move(scene)) {
}

ShadowRenderer::~ShadowRenderer() {
}

void ShadowRenderer::PrepareToDraw() {
  color_program_.reset(new ColorProgram());
  texture_program_.reset(new TextureProgram());
  penumbra_program_.reset(new PenumbraProgram());

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

  Color white(1.0f, 1.0f, 1.0f, 1.0f);
  screen_.AddQuad({ Point(-1.0, -1.0, 0.0), white, gfx::PointF(0.0, 0.0) },
                  { Point( 1.0, -1.0, 0.0), white, gfx::PointF(1.0, 0.0) },
                  { Point( 1.0,  1.0, 0.0), white, gfx::PointF(1.0, 1.0) },
                  { Point(-1.0,  1.0, 0.0), white, gfx::PointF(0.0, 1.0) });
  screen_.BufferData(GL_STATIC_DRAW);
}

void ShadowRenderer::Draw(const gfx::Size size) {
  if (frame_buffer_.is_null())
    frame_buffer_ = FrameBuffer(size);

  // {
    // gfx::ScopedFrameBufferBinder bind(frame_buffer_.id());

    glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, size.width(), size.height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    float aspect = static_cast<GLfloat>(size.width()) / static_cast<GLfloat>(size.height());
    Matrix transform;
    transform.SetPerspective(60.0f, aspect, 1.0f, 20.0f );
    transform.PreTranslate(Offset(0, 0, -2));
    color_program_->Draw(transform, geometry_);
  // }

  // glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
  // glEnable(GL_DEPTH_TEST);
  // glViewport(0, 0, size.width(), size.height());
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // texture_program_->Draw(Matrix(), frame_buffer_.color().id(), screen_);

  // Shadows volumes

  // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthMask(GL_FALSE);
  // glEnable(GL_STENCIL_TEST);
  // glEnable(GL_POLYGON_OFFSET_FILL);
  // glPolygonOffset(0.0f, 100.0f);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // glStencilFunc(GL_ALWAYS, 0x0, 0xff);
  // glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
  // glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
  color_program_->Draw(transform, shadow_);
  glDisable(GL_CULL_FACE);

  // glDisable(GL_POLYGON_OFFSET_FILL);
  // glDisable(GL_CULL_FACE);
  // glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  // Shadows.

  // glDepthMask(GL_FALSE);

  // glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
  // glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

  // glDisable(GL_DEPTH_TEST);
  // color_program_->Draw(Matrix(), shadow_mask_);
  // glEnable(GL_DEPTH_TEST);

  // glDisable(GL_STENCIL_TEST);
  glDepthMask(GL_TRUE);

}

}  // namespace vfx
