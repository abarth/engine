// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/core/compositing/SceneBuilder.h"

#include "flow/layers/backdrop_filter_layer.h"
#include "flow/layers/child_scene_layer.h"
#include "flow/layers/clip_path_layer.h"
#include "flow/layers/clip_rect_layer.h"
#include "flow/layers/clip_rrect_layer.h"
#include "flow/layers/color_filter_layer.h"
#include "flow/layers/container_layer.h"
#include "flow/layers/opacity_layer.h"
#include "flow/layers/performance_overlay_layer.h"
#include "flow/layers/picture_layer.h"
#include "flow/layers/shader_mask_layer.h"
#include "flow/layers/transform_layer.h"
#include "sky/engine/core/compositing/buffer_decoder.h"
#include "sky/engine/core/painting/Matrix.h"
#include "sky/engine/core/painting/Shader.h"
#include "sky/engine/tonic/dart_args.h"
#include "sky/engine/tonic/dart_binding_macros.h"
#include "sky/engine/tonic/dart_converter.h"
#include "sky/engine/tonic/dart_library_natives.h"
#include "third_party/skia/include/core/SkColorFilter.h"

namespace blink {

static void SceneBuilder_constructor(Dart_NativeArguments args) {
  DartCallConstructor(&SceneBuilder::create, args);
}

IMPLEMENT_WRAPPERTYPEINFO(ui, SceneBuilder);

#define FOR_EACH_BINDING(V) \
  V(SceneBuilder, setRasterizerTracingThreshold) \
  V(SceneBuilder, build)

FOR_EACH_BINDING(DART_NATIVE_CALLBACK)

void SceneBuilder::RegisterNatives(DartLibraryNatives* natives) {
  natives->Register({
    { "SceneBuilder_constructor", SceneBuilder_constructor, 1, true },
FOR_EACH_BINDING(DART_REGISTER_NATIVE)
  });
}

SceneBuilder::SceneBuilder()
  : m_currentLayer(nullptr), m_currentRasterizerTracingThreshold(0) {
}

SceneBuilder::~SceneBuilder() { }

void SceneBuilder::pushTransform(const SkMatrix& matrix) {
  std::unique_ptr<flow::TransformLayer> layer(new flow::TransformLayer());
  layer->set_transform(matrix);
  addLayer(std::move(layer));
}

void SceneBuilder::pushClipRect(const SkRect& rect) {
  std::unique_ptr<flow::ClipRectLayer> layer(new flow::ClipRectLayer());
  layer->set_clip_rect(rect);
  addLayer(std::move(layer));
}

void SceneBuilder::pushClipRRect(const SkRRect& rrect) {
  std::unique_ptr<flow::ClipRRectLayer> layer(new flow::ClipRRectLayer());
  layer->set_clip_rrect(rrect);
  addLayer(std::move(layer));
}

void SceneBuilder::pushClipPath(const CanvasPath* path) {
  std::unique_ptr<flow::ClipPathLayer> layer(new flow::ClipPathLayer());
  layer->set_clip_path(path->path());
  addLayer(std::move(layer));
}

void SceneBuilder::pushOpacity(int alpha) {
  std::unique_ptr<flow::OpacityLayer> layer(new flow::OpacityLayer());
  layer->set_alpha(alpha);
  addLayer(std::move(layer));
}

void SceneBuilder::pushColorFilter(int color, int transfer_mode) {
  std::unique_ptr<flow::ColorFilterLayer> layer(new flow::ColorFilterLayer());
  layer->set_color(static_cast<SkColor>(color));
  layer->set_transfer_mode(static_cast<SkXfermode::Mode>(transfer_mode));
  addLayer(std::move(layer));
}

void SceneBuilder::pushBackdropFilter(ImageFilter* filter) {
  std::unique_ptr<flow::BackdropFilterLayer> layer(new flow::BackdropFilterLayer());
  layer->set_filter(filter->toSkia());
  addLayer(std::move(layer));
}

void SceneBuilder::pushShaderMask(Shader* shader,
                                  const SkRect& mask_rect,
                                  int transfer_mode) {
  std::unique_ptr<flow::ShaderMaskLayer> layer(new flow::ShaderMaskLayer());
  layer->set_shader(shader->shader());
  layer->set_mask_rect(mask_rect);
  layer->set_transfer_mode(static_cast<SkXfermode::Mode>(transfer_mode));
  addLayer(std::move(layer));
}

void SceneBuilder::addLayer(std::unique_ptr<flow::ContainerLayer> layer)
{
  DCHECK(layer);
  if (!m_rootLayer) {
    DCHECK(!m_currentLayer);
    m_rootLayer = std::move(layer);
    m_currentLayer = m_rootLayer.get();
    return;
  }
  if (!m_currentLayer)
    return;
  flow::ContainerLayer* newLayer = layer.get();
  m_currentLayer->Add(std::move(layer));
  m_currentLayer = newLayer;
}

void SceneBuilder::pop()
{
  if (!m_currentLayer)
    return;
  m_currentLayer = m_currentLayer->parent();
}

void SceneBuilder::addPicture(double dx, double dy, Picture* picture)
{
    if (!m_currentLayer)
        return;
    std::unique_ptr<flow::PictureLayer> layer(new flow::PictureLayer());
    layer->set_offset(SkPoint::Make(dx, dy));
    layer->set_picture(picture->toSkia());
    m_currentLayer->Add(std::move(layer));
}

void SceneBuilder::addChildScene(double dx,
                                 double dy,
                                 double devicePixelRatio,
                                 int physicalWidth,
                                 int physicalHeight,
                                 uint32_t sceneToken) {
    if (!m_currentLayer)
        return;
    std::unique_ptr<flow::ChildSceneLayer> layer(new flow::ChildSceneLayer());
    layer->set_offset(SkPoint::Make(dx, dy));
    layer->set_device_pixel_ratio(devicePixelRatio);
    layer->set_physical_size(SkISize::Make(physicalWidth, physicalHeight));
    mojo::gfx::composition::SceneTokenPtr token = mojo::gfx::composition::SceneToken::New();
    token->value = sceneToken;
    layer->set_scene_token(token.Pass());
    m_currentLayer->Add(std::move(layer));
}

void SceneBuilder::addPerformanceOverlay(uint64_t enabledOptions, SkRect rect) {
  if (!m_currentLayer)
    return;
  std::unique_ptr<flow::PerformanceOverlayLayer> layer(new flow::PerformanceOverlayLayer(enabledOptions));
  layer->set_paint_bounds(rect);
  m_currentLayer->Add(std::move(layer));
}

void SceneBuilder::setRasterizerTracingThreshold(uint32_t frameInterval) {
  m_currentRasterizerTracingThreshold = frameInterval;
}

// Keep in sync with _SceneRecordType in compositing.dart.
enum SceneRecordType {
  SceneRecordType_pushTransform,
  SceneRecordType_pushClipRect,
  SceneRecordType_pushClipRRect,
  SceneRecordType_pushClipPath,
  SceneRecordType_pushOpacity,
  SceneRecordType_pushColorFilter,
  SceneRecordType_pushBackdropFilter,
  SceneRecordType_pushShaderMask,
  SceneRecordType_pop,
  SceneRecordType_addPerformanceOverlay,
  SceneRecordType_addPicture,
  SceneRecordType_addChildScene,
};

PassRefPtr<Scene> SceneBuilder::build(const ByteData& buffer,
                                      const std::vector<Dart_Handle>& objects)
{
  TRACE_EVENT1("flutter", "SceneBuilder::build", "buffer.length_in_bytes", buffer.length_in_bytes());
  size_t next_object_index = 0;
  std::vector<char> data = buffer.Copy();
  buffer.Release();
  BufferDecoder decoder(data.data(), data.size());
  while (!decoder.is_empty()) {
    switch (decoder.ReadInt64()) {
      case SceneRecordType_pushTransform:
        pushTransform(decoder.ReadSkMatrix());
        break;
      case SceneRecordType_pushClipRect:
        pushClipRect(decoder.ReadSkRect());
        break;
      case SceneRecordType_pushClipRRect:
        pushClipRRect(decoder.ReadSkRRect());
        break;
      case SceneRecordType_pushClipPath:
        pushClipPath(DartConverter<CanvasPath*>::FromDart(objects[next_object_index++]));
        break;
      case SceneRecordType_pushOpacity:
        pushOpacity(decoder.ReadInt());
        break;
      case SceneRecordType_pushColorFilter: {
        int color = decoder.ReadInt();
        int transfer_mode = decoder.ReadInt();
        pushColorFilter(color, transfer_mode);
        break;
      }
      case SceneRecordType_pushBackdropFilter:
        pushBackdropFilter(DartConverter<ImageFilter*>::FromDart(objects[next_object_index++]));
        break;
      case SceneRecordType_pushShaderMask: {
        Shader* shader = DartConverter<Shader*>::FromDart(objects[next_object_index++]);
        SkRect mask_rect = decoder.ReadSkRect();
        int transfer_mode = decoder.ReadInt();
        pushShaderMask(shader, mask_rect, transfer_mode);
        break;
      }
      case SceneRecordType_pop:
        pop();
        break;
      case SceneRecordType_addPerformanceOverlay: {
        int options = decoder.ReadInt();
        SkRect rect = decoder.ReadSkRect();
        addPerformanceOverlay(options, rect);
        break;
      }
      case SceneRecordType_addPicture: {
        double dx = decoder.ReadFloat64();
        double dy = decoder.ReadFloat64();
        Picture* picture = DartConverter<Picture*>::FromDart(objects[next_object_index++]);
        addPicture(dx, dy, picture);
        break;
      }
      case SceneRecordType_addChildScene: {
        double dx = decoder.ReadFloat64();
        double dy = decoder.ReadFloat64();
        double device_pixel_ratio = decoder.ReadFloat64();
        int physical_width = decoder.ReadInt();
        int physical_height = decoder.ReadInt();
        uint32_t scene_token = decoder.ReadUInt32();
        addChildScene(dx, dy, device_pixel_ratio, physical_width, physical_height, scene_token);
        break;
      }
      default:
        CHECK(false) << "Unknown SceneRecordType";
    }
  }

  m_currentLayer = nullptr;
  int32_t threshold = m_currentRasterizerTracingThreshold;
  m_currentRasterizerTracingThreshold = 0;
  RefPtr<Scene> scene = Scene::create(std::move(m_rootLayer), threshold);
  ClearDartWrapper();
  return scene.release();
}

} // namespace blink
