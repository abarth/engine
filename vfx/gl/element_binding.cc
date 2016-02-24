// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "vfx/gl/element_binding.h"

namespace vfx {

ElementBinding::ElementBinding()
  : vertex_buffer_(0),
    index_buffer_(0) {
  glGenBuffersARB(1, &vertex_buffer_);
  glGenBuffersARB(1, &index_buffer_);
}

ElementBinding::~ElementBinding() {
}

void ElementBinding::Bind() {
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
}

}  // namespace vfx
