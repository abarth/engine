// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package io.flutter.shell;

import android.app.Activity;
import android.os.Bundle;

import io.flutter.view.FlutterMain;
import io.flutter.view.FlutterView;

/**
 * Default Flutter implementation of {@link android.app.Activity}.
 */
public class FlutterActivity extends Activity {
    private FlutterView mView;

    /**
     * @see android.app.Activity#onCreate(android.os.Bundle)
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mView = new FlutterView(this);
        setContentView(mView);
    }
}
