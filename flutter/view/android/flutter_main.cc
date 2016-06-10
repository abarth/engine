// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/view/android/flutter_main.h"

#include "base/android/jni_android.h"
#include "base/android/jni_array.h"
#include "base/android/jni_string.h"
#include "base/at_exit.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "base/path_service.h"
#include "base/run_loop.h"
#include "base/threading/simple_thread.h"
#include "flutter/content/content_init.h"
#include "jni/FlutterMain_jni.h"

#include <android/log.h>

using base::LazyInstance;

namespace flutter {

namespace {

LazyInstance<std::unique_ptr<base::MessageLoop>> g_java_message_loop =
    LAZY_INSTANCE_INITIALIZER;

void InitializeLogging() {
  logging::LoggingSettings settings;
  settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG;
  logging::InitLogging(settings);
  // To view log output with IDs and timestamps use "adb logcat -v threadtime".
  logging::SetLogItems(false,   // Process ID
                       false,   // Thread ID
                       false,   // Timestamp
                       false);  // Tick count
}

}  // namespace

static void Init(JNIEnv* env,
                 jclass clazz,
                 jobject context,
                 jobjectArray jargs) {
  __android_log_write(ANDROID_LOG_INFO, "Flutter", "FlutterMain::Init");

  base::PlatformThread::SetName("java_ui_thread");
  base::android::ScopedJavaLocalRef<jobject> scoped_context(env, context);
  base::android::InitApplicationContext(env, scoped_context);

  std::vector<std::string> args;
  args.push_back("flutter_view");
  base::android::AppendJavaStringArrayToStringVector(env, jargs, &args);

  base::CommandLine::Init(0, nullptr);
  base::CommandLine::ForCurrentProcess()->InitFromArgv(args);

  InitializeLogging();
  LOG(INFO) << "InitializeLogging";

  g_java_message_loop.Get().reset(new base::MessageLoopForUI);
  base::MessageLoopForUI::current()->Start();

  flutter::InitContent();
  LOG(INFO) << "Init (done)";
}

static void RecordStartTimestamp(JNIEnv* env, jclass jcaller,
    jlong initTimeMillis) {
}

bool RegisterFlutterMain(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

}  // namespace flutter
