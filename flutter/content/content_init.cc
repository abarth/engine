// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/content/content_init.h"

#include "base/android/path_utils.h"
#include "base/bind.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "flutter/content/dart/dart_init.h"
#include "flutter/content/globals.h"
#include "flutter/content/gpu/gpu_init.h"

namespace flutter {
namespace {

void InitOnUIThread() {
  LOG(INFO) << "InitOnUIThread (start)";
  base::FilePath data_dir;
  CHECK(base::android::GetDataDirectory(&data_dir));
  InitDartVM(data_dir);
  LOG(INFO) << "InitOnUIThread (end)";
}

void InitOnGPUThread() {
  InitGPU();
}

}  // namespace

void InitContent() {
  LOG(INFO) << "InitContent";
  Globals::Init();
  Globals& globals = Globals::Shared();
  LOG(INFO) << "PostTask InitOnUIThread";
  globals.ui()->PostTask(FROM_HERE, base::Bind(&InitOnUIThread));

  // TODO(abarth): Move to GPU thread.
  InitOnGPUThread();
}

}  // namespace flutter
