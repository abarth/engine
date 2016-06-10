// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/content/dart/dart_init.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"
#include "base/files/file.h"

namespace flutter {
namespace {

static uint8_t* g_instructions_snapshot;
static uint8_t* g_isolate_snapshot;
static uint8_t* g_vmisolate_snapshot;
static uint8_t* g_data_snapshot;

uint8_t* LoadSnapshot(const std::string& path, bool is_executable) {
  CHECK(!path.empty());

  int fd = HANDLE_EINTR(::open(path.c_str(), O_RDONLY));
  if (fd == -1) {
    LOG(FATAL) << "Failed to open " << path;
    return nullptr;
  }

  base::stat_wrapper_t file_info;
  HANDLE_EINTR(::fstat64(fd, &file_info));
  int64_t file_size = file_info.st_size;

  int mmap_flags = PROT_READ;
  if (is_executable)
    mmap_flags |= PROT_EXEC;

  void* data = ::mmap(nullptr, file_size, mmap_flags, MAP_PRIVATE, fd, 0);
  if (data == MAP_FAILED) {
    HANDLE_EINTR(::close(fd));
    LOG(FATAL) << "Failed to mmap " << path;
    return nullptr;
  }

  return static_cast<uint8_t*>(data);
}

void LoadAllSnapshots(const base::FilePath& snapshot_dir) {
  LOG(INFO) << "LoadAllSnapshots (start)";
  std::string snapshot_aot_instr =
      snapshot_dir.Append("snapshot_aot_instr").value();
  std::string snapshot_aot_isolate =
      snapshot_dir.Append("snapshot_aot_isolate").value();
  std::string snapshot_aot_vmisolate =
      snapshot_dir.Append("snapshot_aot_vmisolate").value();
  std::string snapshot_aot_rodata =
      snapshot_dir.Append("snapshot_aot_rodata").value();

  g_instructions_snapshot = LoadSnapshot(snapshot_aot_instr, true);
  g_isolate_snapshot = LoadSnapshot(snapshot_aot_isolate, false);
  g_vmisolate_snapshot = LoadSnapshot(snapshot_aot_vmisolate, false);
  g_data_snapshot = LoadSnapshot(snapshot_aot_isolate, false);
  LOG(INFO) << "LoadAllSnapshots (end)";
}

bool IsServiceIsolateURL(const char* script_uri) {
  return script_uri && strcmp(script_uri, DART_VM_SERVICE_ISOLATE_NAME) == 0;
}

void IsolateShutdownCallback(void* callback_data) {
}

void ThreadExitCallback() {
}

Dart_Handle GetVMServiceAssetsArchiveCallback() {
  return nullptr;
}

Dart_Isolate IsolateCreateCallback(const char* script_uri,
                                   const char* main,
                                   const char* package_root,
                                   const char* package_config,
                                   Dart_IsolateFlags* flags,
                                   void* callback_data,
                                   char** error) {
  if (IsServiceIsolateURL(script_uri))
    return nullptr;
  LOG(INFO) << "IsolateCreateCallback (start) script_uri=" << script_uri;

  Dart_Isolate isolate = Dart_CreateIsolate(
      script_uri, main, g_isolate_snapshot, nullptr, nullptr, error);
  LOG(INFO) << "Did call Dart_CreateIsolate";
  CHECK(isolate) << error;
  Dart_ExitIsolate();

  CHECK(Dart_IsolateMakeRunnable(isolate));
  LOG(INFO) << "IsolateCreateCallback (end)";
  return isolate;
}

}  // namespace

void InitDartVM(const base::FilePath& snapshot_dir) {
  LoadAllSnapshots(snapshot_dir);

  CHECK(Dart_SetVMFlags(0, nullptr));

  char* init_error = Dart_Initialize(
      g_vmisolate_snapshot,
      g_instructions_snapshot,
      g_data_snapshot,
      IsolateCreateCallback,
      nullptr,  // Isolate interrupt callback.
      nullptr,
      IsolateShutdownCallback,
      ThreadExitCallback,
      // File IO callbacks.
      nullptr, nullptr, nullptr, nullptr,
      // Entroy source
      nullptr,
      // VM service assets archive
      GetVMServiceAssetsArchiveCallback);
  LOG_IF(ERROR, init_error != nullptr)
      << "Error while initializing the Dart VM: " << init_error;
  CHECK(init_error == nullptr);
  free(init_error);

  char* error = nullptr;
  Dart_Isolate isolate = Dart_CreateIsolate(
      "about:blank", "main", g_isolate_snapshot, nullptr, nullptr, &error);
  CHECK(isolate) << error;
  Dart_ExitIsolate();
  LOG(INFO) << "InitDartVM (end)";
}

}  // namespace flutter
