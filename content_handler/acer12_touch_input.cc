// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/content_handler/acer12_touch_input.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <vector>

#include <magenta/types.h>
#include <magenta/device/display.h>
#include <magenta/device/input.h>
#include <magenta/syscalls.h>
#include <mxio/io.h>

#include <hid/acer12.h>
#include <hid/usages.h>

namespace flutter_content_handler {
namespace {

constexpr char kDevInput[] = "/dev/class/input";

ftl::UniqueFD GetTouchFD() {
  DIR* dir = ::opendir(kDevInput);
  if (!dir)
    return -1;

  std::string device_dir = kDevInput;
  device_dir += "/";

  struct dirent* dir_entry = nullptr;
  while ((dir_entry = readdir(dir)) != nullptr) {
    std::string device_path = device_dir + dir_entry->d_name;
    ftl::UniqueFD fd(::open(device_path.c_str(), O_RDONLY));
    if (!fd.is_valid())
      continue;

    size_t report_desc_len = 0;
    ssize_t ret =
        mxio_ioctl(fd.get(), IOCTL_INPUT_GET_REPORT_DESC_SIZE, nullptr, 0,
                   &report_desc_len, sizeof(report_desc_len));
    if (ret < 0)
      continue;

    if (report_desc_len != ACER12_RPT_DESC_LEN)
      continue;

    std::vector<uint8_t> report_desc(report_desc_len);

    ret = mxio_ioctl(fd.get(), IOCTL_INPUT_GET_REPORT_DESC, nullptr, 0,
                     report_desc.data(), report_desc.size());

    if (ret < 0)
      continue;

    if (!memcmp(report_desc.data(), acer12_touch_report_desc,
                ACER12_RPT_DESC_LEN)) {
      closedir(dir);
      return std::move(fd);
    }
  }
  closedir(dir);
  return ftl::UniqueFD();
}

}  // namespace

Acer12TouchInput::Acer12TouchInput() = default;

Acer12TouchInput::~Acer12TouchInput() = default;

void Acer12TouchInput::Start() {
  fd_ = GetTouchFD();
  if (!fd_.is_valid())
    return;

  size_t max_report_len = 0;
  ssize_t ret = mxio_ioctl(fd_.get(), IOCTL_INPUT_GET_MAX_REPORTSIZE, nullptr,
                           0, &max_report_len, sizeof(max_report_len));
  if (ret < 0)
    return;

  buffer_.resize(max_report_len);
  // TODO(abarth): Watch for events.
}

void Acer12TouchInput::DidBecomeReadable() {
  ssize_t ret = ::read(fd_.get(), buffer_.data(), buffer_.size());
  if (ret < 0)
    return;
  if (buffer_[0] != ACER12_RPT_ID_TOUCH)
    return;

  auto packet = pointer::PointerPacket::New();

  acer12_touch_t* report = reinterpret_cast<acer12_touch_t*>(buffer_.data());

  for (uint8_t i = 0; i < 5; ++i) {
    const acer12_finger& finger = report->fingers[i];
    if (!finger.finger_id)
      continue;
    int id = acer12_finger_id_contact(finger.finger_id);
    int x = finger.x * width_ / ACER12_X_MAX;
    int y = finger.y * height_ / ACER12_Y_MAX;

    // if (!acer12_finger_id_tswitch(finger.finger_id))
    //   continue;
  }
}

}  // namespace flutter_content_handler
