// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/sky/shell/gpu/software/rasterizer_software.h"

#include "flutter/glue/trace_event.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include "flutter/sky/engine/wtf/PassRefPtr.h"
#include "flutter/sky/engine/wtf/RefPtr.h"
#include "flutter/sky/shell/gpu/picture_serializer.h"
#include "flutter/sky/shell/platform_view.h"
#include "flutter/sky/shell/shell.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPicture.h"

namespace sky {
namespace shell {

RasterizerSoftware::RasterizerSoftware()
    : binding_(this), weak_factory_(this) {}

RasterizerSoftware::~RasterizerSoftware() {
  weak_factory_.InvalidateWeakPtrs();
  Shell::Shared().PurgeRasterizers();
}

void RasterizerSoftware::Init(mojo::ApplicationConnectorPtr connector) {
  // TODO(abarth): Connect to the framebuffer service and set up surface_.
}

// Implementation of declaration in sky/shell/rasterizer.h.
std::unique_ptr<Rasterizer> Rasterizer::Create() {
  return std::unique_ptr<Rasterizer>(new RasterizerSoftware());
}

// sky::shell::Rasterizer override.
ftl::WeakPtr<Rasterizer> RasterizerSoftware::GetWeakRasterizerPtr() {
  return weak_factory_.GetWeakPtr();
}

// sky::shell::Rasterizer override.
void RasterizerSoftware::ConnectToRasterizer(
    mojo::InterfaceRequest<rasterizer::Rasterizer> request) {
  binding_.Bind(request.Pass());

  Shell::Shared().AddRasterizer(GetWeakRasterizerPtr());
}

// sky::shell::Rasterizer override.
void RasterizerSoftware::Setup(
    PlatformView* platform_view,
    ftl::Closure continuation,
    ftl::AutoResetWaitableEvent* setup_completion_event) {
  setup_completion_event->Signal();
  continuation();
}

// sky::shell::Rasterizer override.
void RasterizerSoftware::Teardown(
    ftl::AutoResetWaitableEvent* teardown_completion_event) {
  last_layer_tree_.reset();
  teardown_completion_event->Signal();
}

// sky::shell::Rasterizer override.
flow::LayerTree* RasterizerSoftware::GetLastLayerTree() {
  return last_layer_tree_.get();
}

void RasterizerSoftware::Draw(uint64_t layer_tree_ptr,
                              const DrawCallback& callback) {
  TRACE_EVENT0("flutter", "RasterizerSoftware::Draw");

  std::unique_ptr<flow::LayerTree> layer_tree(
      reinterpret_cast<flow::LayerTree*>(layer_tree_ptr));

  SkISize size = layer_tree->frame_size();

  if (!layer_tree->root_layer()) {
    callback.Run();
    return;
  }

  {
    SkCanvas* canvas = surface_->getCanvas();
    flow::CompositorContext::ScopedFrame frame =
        compositor_context_.AcquireFrame(nullptr, *canvas);
    canvas->clear(SK_ColorBLACK);
    layer_tree->Raster(frame);
  }

  callback.Run();
  last_layer_tree_ = std::move(layer_tree);
}

}  // namespace shell
}  // namespace sky
