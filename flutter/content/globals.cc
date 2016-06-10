// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/content/globals.h"

#include "base/bind.h"
#include "base/single_thread_task_runner.h"

namespace flutter {
namespace {

static Globals* g_instance = nullptr;

}  // namespace

Globals::Globals() {
  DCHECK(!g_instance);

  base::Thread::Options options;

  ui_thread_.reset(new base::Thread("ui_thread"));
  ui_thread_->StartWithOptions(options);
  ui_task_runner_ = ui_thread_->message_loop()->task_runner();
}

Globals::~Globals() {}

void Globals::Init() {
  CHECK(!g_instance);
  g_instance = new Globals();
}

Globals& Globals::Shared() {
  DCHECK(g_instance);
  return *g_instance;
}

}  // namespace flutter
