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

#include <magenta/device/device.h>
#include <magenta/device/input.h>
#include <magenta/syscalls.h>
#include <magenta/types.h>
#include <mxio/io.h>

#include <hid/acer12.h>
#include <hid/usages.h>

#include "lib/ftl/time/time_point.h"

namespace flutter_content_handler {
namespace {

constexpr char kDevInput[] = "/dev/class/input";

ftl::UniqueFD GetTouchFD() {
  DIR* dir = ::opendir(kDevInput);
  if (!dir)
    return ftl::UniqueFD();

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
      return fd;
    }
  }
  closedir(dir);
  return ftl::UniqueFD();
}

void WaitComplete(void* closure, MojoResult result) {
  static_cast<Acer12TouchInput*>(closure)->DidBecomeReadable();
}

}  // namespace

Acer12TouchInput::Acer12TouchInput(const MojoAsyncWaiter* waiter)
    : waiter_(waiter) {}

Acer12TouchInput::~Acer12TouchInput() {
  if (wait_id_)
    waiter_->CancelWait(wait_id_);
}

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

  mx_handle_t input_event = 0;
  ret = mxio_ioctl(fd_.get(), IOCTL_DEVICE_GET_EVENT_HANDLE, nullptr, 0,
                   &input_event, sizeof(input_event));

  if (ret < 0)
    return;

  input_event_.reset(input_event);
  WaitForInput();
}

void Acer12TouchInput::WaitForInput() {
  wait_id_ =
      waiter_->AsyncWait(MojoHandle(input_event_.get()), DEVICE_SIGNAL_READABLE,
                         MOJO_DEADLINE_INDEFINITE, WaitComplete, this);
}

void Acer12TouchInput::DidBecomeReadable() {
  ssize_t ret = ::read(fd_.get(), buffer_.data(), buffer_.size());
  if (ret < 0)
    return;
  if (buffer_[0] != ACER12_RPT_ID_TOUCH)
    return;

  std::vector<blink::PointerData> pointer_packet;
  acer12_touch_t* report = reinterpret_cast<acer12_touch_t*>(buffer_.data());

  // The first packet gives us the total count of all contacts. The second
  // packet will have a count of zero, which means we need to remember the count
  // from the previous packet and subtract the five we already processed.
  // In any case, we should never have a count for a single package that's
  // greater than 5 because that's all the room there is in a single packet.
  int count = report->contact_count;
  if (!count)
    count = last_contact_count_ - 5;
  last_contact_count_ = report->contact_count;
  if (count > 5)
    count = 5;

  // TODO(abarth): Consider using the scan time, which is in a different time
  // base.
  int time_stamp = (ftl::TimePoint::Now() - ftl::TimePoint()).ToMicroseconds();

  for (uint8_t i = 0; i < count; ++i) {
    const acer12_finger& finger = report->fingers[i];
    bool down = acer12_finger_id_tswitch(finger.finger_id);
    int id = acer12_finger_id_contact(finger.finger_id);
    int x = finger.x * width_ / ACER12_X_MAX;
    int y = finger.y * height_ / ACER12_Y_MAX;

    // FTL_LOG(INFO) << "id=" << id << " x=" << x << " y" << y << " down=" <<
    // down;

    bool was_down = down_pointers_.count(id) > 0;
    if (down)
      down_pointers_.insert(id);
    else
      down_pointers_.erase(id);

    blink::PointerData pointer_data;
    pointer_data.time_stamp = time_stamp;
    pointer_data.pointer = id;
    if (!down)
      pointer_data.change = blink::PointerData::Change::kUp;
    else if (was_down)
      pointer_data.change = blink::PointerData::Change::kMove;
    else
      pointer_data.change = blink::PointerData::Change::kDown;
    pointer_data.kind = blink::PointerData::DeviceKind::kTouch;
    pointer_data.physical_x = x;
    pointer_data.physical_y = y;
    pointer_data.pressure = 1.0;
    pointer_data.pressure_max = 1.0;

    pointer_packet.push_back(pointer_data);
  }

  if (event_sink_) {
    event_sink_(std::move(blink::PointerDataPacket(
        reinterpret_cast<char*>(pointer_packet.data()),
        pointer_packet.size() * sizeof(blink::PointerData))));
  }
  WaitForInput();
}

}  // namespace flutter_content_handler
