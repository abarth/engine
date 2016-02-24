// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VFX_GEOMETRY_ELEMENT_ARRAY_H_
#define VFX_GEOMETRY_ELEMENT_ARRAY_H_

#include <memory>
#include <vector>

namespace vfx {

template<typename Vertex>
class ElementArray {
 public:
  ElementArray() { }
  ~ElementArray() { }

  ElementArray(const ElementArray& other) = delete;
  ElementArray(ElementArray&& other)
    : vertices_(std::move(other.vertices_)),
      indices_(std::move(other.indices_)) { }

  ElementArray& operator=(const ElementArray& other) = delete;
  ElementArray& operator=(ElementArray&& other) {
    vertices_ = std::move(other.vertices_);
    indices_ = std::move(other.indices_);
    return *this;
  }

  void AddVertex(Vertex vertex) {
    vertices_.push_back(std::move(vertex));
  }

  void AddIndices(uint8_t a, uint8_t b, uint8_t c) {
    indices_.push_back(a);
    indices_.push_back(b);
    indices_.push_back(c);
  }

  void AddTriange(Vertex p1, Vertex p2, Vertex p3) {
    uint16_t base = vertices_.size();

    vertices_.push_back(std::move(p1));
    vertices_.push_back(std::move(p2));
    vertices_.push_back(std::move(p3));

    indices_.push_back(base + 0);
    indices_.push_back(base + 1);
    indices_.push_back(base + 2);
  }

  void AddQuad(Vertex p1, Vertex p2, Vertex p3, Vertex p4) {
    uint16_t base = vertices_.size();

    vertices_.push_back(std::move(p1));
    vertices_.push_back(std::move(p2));
    vertices_.push_back(std::move(p3));
    vertices_.push_back(std::move(p4));

    indices_.push_back(base + 0);
    indices_.push_back(base + 1);
    indices_.push_back(base + 2);

    indices_.push_back(base + 2);
    indices_.push_back(base + 3);
    indices_.push_back(base + 0);
  }

  const Vertex* vertices() const { return vertices_.data(); }
  size_t verticesSize() const { return vertices_.size(); }

  const uint16_t* indices() const { return indices_.data(); }
  size_t indicesSize() const { return indices_.size(); }

 private:
  std::vector<Vertex> vertices_;
  std::vector<uint16_t> indices_;
};

}  // namespace vfx

#endif  // VFX_GEOMETRY_ELEMENT_ARRAY_H_
