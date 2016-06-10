// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_VIEW_ANDROID_FLUTTER_VIEW_H_
#define FLUTTER_VIEW_ANDROID_FLUTTER_VIEW_H_

#include "base/android/jni_android.h"
#include "base/macros.h"

struct ANativeWindow;

namespace flutter {

class FlutterView {
 public:
  static bool Register(JNIEnv* env);

  FlutterView();
  ~FlutterView();

  // Called from Java
  void Detach(JNIEnv* env, jobject obj);
  void SurfaceCreated(JNIEnv* env, jobject obj, jobject jsurface);
  void SurfaceDestroyed(JNIEnv* env, jobject obj);

 private:
  void ReleaseWindow();

  ANativeWindow* window_;

  DISALLOW_COPY_AND_ASSIGN(FlutterView);
};

}  // namespace flutter

#endif  // FLUTTER_VIEW_ANDROID_FLUTTER_VIEW_H_
