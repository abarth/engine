// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

part of dart_ui;

/// An opaque object representing a composited scene.
///
/// To create a Scene object, use a [SceneBuilder].
///
/// Scene objects can be displayed on the screen using the
/// [Window.render] method.
class Scene extends NativeFieldWrapperClass2 {
  /// Creates an uninitialized Scene object.
  ///
  /// Calling the Scene constructor directly will not create a useable
  /// object. To create a Scene object, use a [SceneBuilder].
  Scene(); // (this constructor is here just so we can document it)

  /// Releases the resources used by this scene.
  ///
  /// After calling this function, the scene is cannot be used further.
  void dispose() native "Scene_dispose";
}

class _EncodedBuffer {
  int _nextIndex = 0;
  int _length = _kInitialBufferSize;
  final List<dynamic> objects = <dynamic>[];
  Float64List _float64;
  Int64List _int64;

  static const int _kInitialBufferSize = 1024;

  _EncodedBuffer() {
    _int64 = new Int64List(_kInitialBufferSize);
    _float64 = _int64.buffer.asFloat64List();
  }

  void _grow(int newSize) {
    UInt32List oldBuffer = _int64.buffer.asUint32List();
    UInt32List newBuffer = new Int32List(2 * newSize);
    int newIndex = 0;
    for (int i = 0; i < oldBuffer.length; ++i)
      newBuffer[newIndex++] = oldBuffer[i];
    _length = newSize;
    ByteBuffer buffer = newBuffer.buffer;
    _float64 = buffer.asFloat64List();
    _int64 = buffer.asInt64List();
  }

  int _advance(int count) {
    final int startIndex = _nextIndex;
    _nextIndex += count;
    if (_nextIndex > _length) {
      int newSize = _length + count;
      newSize += (newSize >> 1);
      _grow(newSize);
    }
    return startIndex;
  }

  ByteData get trimmed => new ByteData.view(_int64.buffer, 0, _nextIndex * Int64List.BYTES_PER_ELEMENT);

  void writeFloat64List(Float64List data) {
    final int length = data.length;
    final int startIndex = _advance(length);
    for (int i = 0; i < length; ++i)
      _float64[startIndex + i] = data[i];
  }

  void writeFloat32List(Float32List data) {
    final int length = data.length;
    final int startIndex = _advance(length);
    for (int i = 0; i < length; ++i)
      _float64[startIndex + i] = data[i];
  }

  void writeFloat64(double a) {
    _float64[_advance(1)] = a;
  }

  void write2xFloat64(double a, double b) {
    final int startIndex = _advance(2);
    _float64[startIndex] = a;
    _float64[startIndex + 1] = b;
  }

  void writeInt64(int a) {
    _int64[_advance(1)] = a;
  }
}

// Keep in sync with SceneRecordType in SceneBuilder.cpp.
enum _SceneRecordType {
  pushTransform,
  pushClipRect,
  pushClipRRect,
  pushClipPath,
  pushOpacity,
  pushColorFilter,
  pushBackdropFilter,
  pushShaderMask,
  pop,
  addPerformanceOverlay,
  addPicture,
  addChildScene,
}

/// Builds a [Scene] containing the given visuals.
///
/// A [Scene] can then be rendered using [Window.render].
///
/// To draw graphical operations onto a [Scene], first create a
/// [Picture] using a [PictureRecorder] and a [Canvas], and then add
/// it to the scene using [addPicture].
class SceneBuilder extends NativeFieldWrapperClass2 {
  _EncodedBuffer _recording = new _EncodedBuffer();

  /// Creates an empty [SceneBuilder] object.
  SceneBuilder() { _constructor(); }
  void _constructor() native "SceneBuilder_constructor";

  /// Pushes a transform operation onto the operation stack.
  ///
  /// The objects are transformed by the given matrix before rasterization.
  ///
  /// See [pop] for details about the operation stack.
  void pushTransform(Float64List matrix4) {
    assert(matrix4.length == 16);
    _recording
      ..writeInt64(_SceneRecordType.pushTransform.index)
      ..writeFloat64List(matrix4);
  }

  /// Pushes a rectangular clip operation onto the operation stack.
  ///
  /// Rasterization outside the given rectangle is discarded.
  ///
  /// See [pop] for details about the operation stack.
  void pushClipRect(Rect rect) {
    _recording
      ..writeInt64(_SceneRecordType.pushClipRect.index)
      ..writeFloat32List(rect._value);
  }

  /// Pushes a rounded-rectangular clip operation onto the operation stack.
  ///
  /// Rasterization outside the given rounded rectangle is discarded.
  ///
  /// See [pop] for details about the operation stack.
  void pushClipRRect(RRect rrect) {
    _recording
      ..writeInt64(_SceneRecordType.pushClipRRect.index)
      ..writeFloat32List(rrect._value);
  }

  /// Pushes a path clip operation onto the operation stack.
  ///
  /// Rasterization outside the given path is discarded.
  ///
  /// See [pop] for details about the operation stack.
  void pushClipPath(Path path) {
    _recording
      ..writeInt64(_SceneRecordType.pushClipPath.index)
      ..writeFloat32List(rrect._value);
  }

  /// Pushes an opacity operation onto the operation stack.
  ///
  /// The given alpha value is blended into the alpha value of the objects'
  /// rasterization. An alpha value of 0 makes the objects entirely invisible.
  /// An alpha value of 255 has no effect (i.e., the objects retain the current
  /// opacity).
  ///
  /// See [pop] for details about the operation stack.
  void pushOpacity(int alpha) {
    _recording
      ..writeInt64(_SceneRecordType.pushOpacity.index)
      ..writeInt64(alpha);
  }

  /// Pushes a color filter operation onto the operation stack.
  ///
  /// The given color is applied to the objects' rasterization using the given
  /// transfer mode.
  ///
  /// See [pop] for details about the operation stack.
  void pushColorFilter(Color color, TransferMode transferMode) {
    _recording
      ..writeInt64(_SceneRecordType.pushColorFilter.index)
      ..writeInt64(color._value);
      ..writeInt64(transferMode.index);
  }

  /// Pushes a backdrop filter operation onto the operation stack.
  ///
  /// The given filter is applied to the current contents of the scene prior to
  /// rasterizing the given objects.
  ///
  /// See [pop] for details about the operation stack.
  void pushBackdropFilter(ImageFilter filter) {
    _recording
      ..writeInt64(_SceneRecordType.pushBackdropFilter.index)
      ..objects.add(filter);
  }

  /// Pushes a shader mask operation onto the operation stack.
  ///
  /// The given shader is applied to the object's rasterization in the given
  /// rectangle using the given transfer mode.
  ///
  /// See [pop] for details about the operation stack.
  void pushShaderMask(Shader shader, Rect maskRect, TransferMode transferMode) {
    _recording
      ..writeInt64(_SceneRecordType.pushShaderMask.index)
      ..objects.add(filter)
      ..writeFloat32List(maskRect._value)
      ..writeInt64(transferMode.index);
  }

  /// Ends the effect of the most recently pushed operation.
  ///
  /// Internally the scene builder maintains a stack of operations. Each of the
  /// operations in the stack applies to each of the objects added to the scene.
  /// Calling this function removes the most recently added operation from the
  /// stack.
  void pop() {
    _recording.writeInt64(_SceneRecordType.pop.index);
  }

  /// Adds an object to the scene that displays performance statistics.
  ///
  /// Useful during development to assess the performance of the application.
  /// The enabledOptions controls which statistics are displayed. The bounds
  /// controls where the statistics are displayed.
  ///
  /// enabledOptions is a bit field with the following bits defined:
  ///  - 0x01: displayRasterizerStatistics - show GPU thread frame time
  ///  - 0x02: visualizeRasterizerStatistics - graph GPU thread frame times
  ///  - 0x04: displayEngineStatistics - show UI thread frame time
  ///  - 0x08: visualizeEngineStatistics - graph UI thread frame times
  /// Set enabledOptions to 0x0F to enable all the currently defined features.
  ///
  /// The "UI thread" is the thread that includes all the execution of
  /// the main Dart isolate (the isolate that can call
  /// [Window.render]). The UI thread frame time is the total time
  /// spent executing the [Window.onBeginFrame] callback. The "GPU
  /// thread" is the thread (running on the CPU) that subsequently
  /// processes the [Scene] provided by the Dart code to turn it into
  /// GPU commands and send it to the GPU.
  ///
  /// See also the [PerformanceOverlayOption] enum in the rendering library.
  /// for more details.
  // Values above must match constants in //engine/src/sky/compositor/performance_overlay_layer.h
  void addPerformanceOverlay(int enabledOptions, Rect bounds) {
    _recording
      ..writeInt64(_SceneRecordType.addPerformanceOverlay.index)
      ..writeInt64(enabledOptions)
      ..writeFloat32List(bounds._value);
  }

  /// Adds a [Picture] to the scene.
  ///
  /// The picture is rasterized at the given offset.
  void addPicture(Offset offset, Picture picture) {
    _recording
      ..writeInt64(_SceneRecordType.addPicture.index)
      ..write2xFloat64(offset.dx, offset.dy)
      ..objects.add(picture);
  }

  /// (mojo-only) Adds a scene rendered by another application to the scene for
  /// this application.
  ///
  /// Applications typically obtain scene tokens when embedding other views via
  /// the Mojo view manager, but this function is agnostic as to the source of
  /// scene token.
  void addChildScene(Offset offset,
                     double devicePixelRatio,
                     int physicalWidth,
                     int physicalHeight,
                     int sceneToken) {
    _recording
      ..writeInt64(_SceneRecordType.addChildScene.index)
      ..write2xFloat64(offset.dx, offset.dy)
      ..writeFloat64(devicePixelRatio)
      ..writeInt64(physicalWidth)
      ..writeInt64(physicalHeight)
      ..writeInt64(sceneToken);
  }

  /// Sets a threshold after which additional debugging information should be recorded.
  ///
  /// Currently this interface is difficult to use by end-developers. If you're
  /// interested in using this feature, please contact [flutter-dev](https://groups.google.com/forum/#!forum/flutter-dev).
  /// We'll hopefully be able to figure out how to make this feature more useful
  /// to you.
  void setRasterizerTracingThreshold(int frameInterval) native "SceneBuilder_setRasterizerTracingThreshold";

  /// Finishes building the scene.
  ///
  /// Returns a [Scene] containing the objects that have been added to
  /// this scene builder. The [Scene] can then be displayed on the
  /// screen with [Window.render].
  ///
  /// After calling this function, the scene builder object is invalid and
  /// cannot be used further.
  Scene build() {
    Scene result = _build(_recording.trimmed, _recording.objects);
    _recording = null;
    return result;
  }
  Scene _build(ByteData buffer, List<dynamic> objects) native "SceneBuilder_build";
}
