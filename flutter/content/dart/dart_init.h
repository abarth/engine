// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_CONTENT_DART_DART_INIT_H_
#define FLUTTER_CONTENT_DART_DART_INIT_H_

#include "base/files/file_path.h"
#include "dart/runtime/include/dart_api.h"

namespace flutter {

void InitDartVM(const base::FilePath& snapshot_dir);

}  // namespace flutter

#endif  // FLUTTER_CONTENT_DART_DART_INIT_H_
