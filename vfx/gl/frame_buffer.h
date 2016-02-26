// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_GL_FRAME_BUFFER_H_
#define VFX_GL_FRAME_BUFFER_H_

#include <GL/gl.h>

#include "base/logging.h"

namespace vfx {

class FrameBuffer {
 public:
  FrameBuffer(GLsizei width, GLsizei height);
  ~FrameBuffer();

  void Bind();

 private:
  GLsizei width_;
  GLsizei height_;
  GLuint frame_buffer_;
  GLuint color_buffer_;
  GLuint depth_buffer_;

  DISALLOW_COPY_AND_ASSIGN(FrameBuffer);
};

}  // namespace vfx

#endif  // VFX_GL_FRAME_BUFFER_H_
