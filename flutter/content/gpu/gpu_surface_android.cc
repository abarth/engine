// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/content/content_init.h"

#include <android/native_window.h> 
#include <EGL/egl.h>
#include <GLES/gl.h>

namespace flutter {

void InitContent() {
  base::FilePath data_dir;
  CHECK(base::android::GetDataDirectory(&data_dir));
  InitDartVM(data_dir);
}

}  // namespace flutter


