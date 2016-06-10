// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package io.flutter.shell;

import android.app.Application;

import io.flutter.view.FlutterMain;

/**
 * Default Flutter implementation of {@link android.app.Application}.
 */
public class FlutterApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        FlutterMain.startInitialization(this);
    }
}
