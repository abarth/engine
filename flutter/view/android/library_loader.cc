// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/android/base_jni_onload.h"
#include "base/android/base_jni_registrar.h"
#include "base/android/jni_android.h"
#include "base/android/jni_registrar.h"
#include "base/android/library_loader/library_loader_hooks.h"
#include "base/bind.h"
#include "base/logging.h"
#include "mojo/android/system/base_run_loop.h"
#include "mojo/android/system/core_impl.h"
#include "flutter/view/android/flutter_view.h"
#include "flutter/view/android/flutter_main.h"

#include <android/log.h>

namespace {

base::android::RegistrationMethod kRegisteredMethods[] = {
    {"FlutterView", flutter::FlutterView::Register},
    {"FlutterMain", flutter::RegisterFlutterMain},
};

bool RegisterJNI(JNIEnv* env) {
  if (!base::android::RegisterJni(env))
    return false;

  return RegisterNativeMethods(env, kRegisteredMethods,
                               arraysize(kRegisteredMethods));
}

}  // namespace

// This is called by the VM when the shared library is first loaded.
JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  __android_log_write(ANDROID_LOG_INFO, "Flutter", "JNI_OnLoad (start)");

  std::vector<base::android::RegisterCallback> register_callbacks;
  register_callbacks.push_back(base::Bind(&RegisterJNI));

  std::vector<base::android::InitCallback> init_callbacks;

  if (!base::android::OnJNIOnLoadRegisterJNI(vm, register_callbacks) ||
      !base::android::OnJNIOnLoadInit(init_callbacks)) {
    return -1;
  }

  __android_log_write(ANDROID_LOG_INFO, "Flutter", "JNI_OnLoad (end)");

  return JNI_VERSION_1_4;
}
