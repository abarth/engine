// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_CONTENT_HANDLER_HID_H_
#define FLUTTER_CONTENT_HANDLER_HID_H_

#include <thread>

namespace flutter_content_handler {

class Acer12TouchInput {
 public:
  class Delegate {
   public:
    void DidReceivePointerPacket();
  };

  explicit Acer12TouchInput(Delegate* delegate);
  ~Acer12TouchInput();

  void Start();

 private:
  std::thread wait_thread_;

  FTL_DISALLOW_COPY_AND_ASSIGN(Acer12TouchInput);
};

}  // namespace flutter_content_handler

#endif  // FLUTTER_CONTENT_HANDLER_HID_H_
