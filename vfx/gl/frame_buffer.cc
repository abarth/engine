// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/gl/frame_buffer.h"

#include "ui/gl/gl_bindings.h"

namespace vfx {

FrameBuffer::FrameBuffer(GLsizei width, GLsizei height)
  : width_(width), height_(height), frame_buffer_(0), color_buffer_(0),
    depth_buffer_(0) {
  glGenTextures(1, &color_buffer_);
  glBindTexture(GL_TEXTURE_2D, color_buffer_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  glGenTextures(1, &depth_buffer_);
  glBindTexture(GL_TEXTURE_2D, depth_buffer_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

  glGenFramebuffersEXT(1, &frame_buffer_);
  glBindFramebufferEXT(GL_FRAMEBUFFER, frame_buffer_);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer_, 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer_, 0);

  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
    LOG(FATAL) << "Failed to create frame buffer: " << status;
}

FrameBuffer::~FrameBuffer() {
  if (color_buffer_)
    glDeleteTextures(1, &color_buffer_);
  if (depth_buffer_)
    glDeleteTextures(1, &depth_buffer_);
  if (frame_buffer_)
    glDeleteFramebuffersEXT(1, &frame_buffer_);
}

void FrameBuffer::Bind() {
  glBindFramebufferEXT(GL_FRAMEBUFFER, frame_buffer_);
}

}  // namespace vfx
