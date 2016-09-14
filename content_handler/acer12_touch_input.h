// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_CONTENT_HANDLER_ACER12_TOUCH_INPUT_H_
#define FLUTTER_CONTENT_HANDLER_ACER12_TOUCH_INPUT_H_

#include <functional>
#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

#include <mojo/environment/async_waiter.h>

#include "flutter/services/pointer/pointer.mojom.h"
#include "lib/ftl/files/unique_fd.h"
#include "lib/mtl/handles/unique_handle.h"
#include "mojo/public/cpp/environment/async_waiter.h"

namespace flutter_content_handler {

class Acer12TouchInput {
 public:
  Acer12TouchInput(const MojoAsyncWaiter* waiter =
                       mojo::Environment::GetDefaultAsyncWaiter());
  ~Acer12TouchInput();

  void set_event_sink(
      std::function<void(pointer::PointerPacketPtr packet)> event_sink) {
    event_sink_ = std::move(event_sink);
  }

  void set_width(int width) { width_ = width; }
  void set_height(int height) { height_ = height; }

  void Start();
  void DidBecomeReadable();

 private:
  void WaitForInput();

  const MojoAsyncWaiter* waiter_;
  std::function<void(pointer::PointerPacketPtr packet)> event_sink_;
  ftl::UniqueFD fd_;

  std::vector<uint8_t> buffer_;
  int width_ = 0;
  int height_ = 0;

  mtl::UniqueHandle input_event_;
  MojoAsyncWaitID wait_id_ = 0;

  std::unordered_set<int> down_pointers_;
  int last_contact_count_ = 0;

  FTL_DISALLOW_COPY_AND_ASSIGN(Acer12TouchInput);
};

}  // namespace flutter_content_handler

#endif  // FLUTTER_CONTENT_HANDLER_ACER12_TOUCH_INPUT_H_
