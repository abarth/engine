// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/content/content_init.h"

#include "base/android/path_utils.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "flutter/content/dart/dart_init.h"

namespace flutter {

void InitContent() {
  base::FilePath data_dir;
  CHECK(base::android::GetDataDirectory(&data_dir));
  InitDartVM(data_dir);
}

}  // namespace flutter
