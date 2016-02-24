// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_GL_ELEMENT_BINDING_H_
#define VFX_GL_ELEMENT_BINDING_H_

#include "ui/gl/gl_bindings.h"
#include "vfx/geometry/element_array.h"

namespace vfx {

class ElementBinding {
 public:
  ElementBinding();
  ~ElementBinding();

  template<typename Vertex>
  void BufferData(const ElementArray<Vertex>& elements, GLenum usage) {
    count_ = elements.index_count();
    GLsizeiptr vertex_size = elements.vertex_count() * sizeof(Vertex);
    GLsizeiptr index_size = elements.index_count() * sizeof(typename ElementArray<Vertex>::Index);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, vertex_size, elements.vertex_data(), usage);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, elements.index_data(), usage);
  }

  void Bind();

  GLsizei count() const { return count_; }

 private:
  GLuint vertex_buffer_;
  GLuint index_buffer_;
  GLsizei count_;
};

}  // namespace vfx

#endif  // VFX_GL_ELEMENT_BINDING_H_
