// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_CONTENT_GLOBALS_H_
#define FLUTTER_CONTENT_GLOBALS_H_

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop/message_loop.h"
#include "base/threading/thread.h"

namespace flutter {

class Globals {
 public:
  ~Globals();

  static void Init();

  static Globals& Shared();

  base::SingleThreadTaskRunner* ui() const {
    return ui_task_runner_.get();
  }

 private:
  Globals();

  std::unique_ptr<base::Thread> ui_thread_;

  scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner_;

  DISALLOW_COPY_AND_ASSIGN(Globals);
};

}  // namespace flutter

#endif  // FLUTTER_CONTENT_GLOBALS_H_
