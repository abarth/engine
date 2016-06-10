// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/content/gpu/gpu_init.h"

#include <EGL/egl.h>
#include <GLES/gl.h>

namespace flutter {
namespace {

EGLDisplay g_display;

}  // namespace


void InitGPU() {
  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  CHECK(display != EGL_NO_DISPLAY)
      << "eglGetDisplay error: " << eglGetError();

  CHECK(eglInitialize(display, nullptr, nullptr))
      << "eglInitialize error: " << eglGetError();
}


{
  const EGLint attribs[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT | EGL_OPENGL_ES3_BIT,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BUFFER_SIZE, 32,
    EGL_ALPHA_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_STENCIL_SIZE, 8,
    EGL_NONE
  };
  EGLConfig config;    
  EGLint numConfigs;
  EGLint format;
  EGLSurface surface;
  EGLContext context;
  EGLint width;
  EGLint height;
  GLfloat ratio;

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        LOG_ERROR("eglChooseConfig() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOG_ERROR("eglGetConfigAttrib() returned error %d", eglGetError());
        destroy();
        return false;
    }

    ANativeWindow_setBuffersGeometry(_window, 0, 0, format);

}

}  // namespace flutter
