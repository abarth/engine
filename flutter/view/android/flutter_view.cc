// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/view/android/flutter_view.h"

#include <android/input.h>
#include <android/native_window_jni.h>

#include "base/android/jni_android.h"
#include "base/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "jni/FlutterView_jni.h"

namespace flutter {

static jlong Attach(JNIEnv* env, jclass clazz, jint skyEngineHandle) {
  FlutterView* view = new FlutterView();
  return reinterpret_cast<jlong>(view);
}

// static
bool FlutterView::Register(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

FlutterView::FlutterView()
  : window_(nullptr) {
  LOG(INFO) << "FlutterView::FlutterView";
}

FlutterView::~FlutterView() {
  if (window_)
    ReleaseWindow();
}

void FlutterView::Detach(JNIEnv* env, jobject obj) {
  delete this;
}

void FlutterView::SurfaceCreated(JNIEnv* env, jobject obj, jobject jsurface) {
  LOG(INFO) << "FlutterView::SurfaceCreated";
  base::android::ScopedJavaLocalRef<jobject> protector(env, jsurface);
  // Note: This ensures that any local references used by
  // ANativeWindow_fromSurface are released immediately. This is needed as a
  // workaround for https://code.google.com/p/android/issues/detail?id=68174
  {
    base::android::ScopedJavaLocalFrame scoped_local_reference_frame(env);
    window_ = ANativeWindow_fromSurface(env, jsurface);
  }
}

void FlutterView::SurfaceDestroyed(JNIEnv* env, jobject obj) {
  ReleaseWindow();
}

void FlutterView::ReleaseWindow() {
  ANativeWindow_release(window_);
  window_ = nullptr;
}

}  // namespace flutter
