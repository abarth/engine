// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package io.flutter.view;

import android.content.Context;
import android.util.Log;
import android.content.res.AssetManager;
import android.os.SystemClock;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;

import org.chromium.base.JNINamespace;
import org.chromium.base.PathUtils;
import org.chromium.base.library_loader.LibraryLoader;
import org.chromium.base.library_loader.LibraryProcessType;
import org.chromium.base.library_loader.ProcessInitException;

/**
 * A class to intialize the Flutter engine.
 **/
@JNINamespace("flutter")
public class FlutterMain {
    private static final String TAG = "Flutter";

    /**
     * Starts initialization of the native system.
     **/
    public static void startInitialization(Context applicationContext) {
        Log.i(TAG, "startInitialization (start)");
        PathUtils.setPrivateDataDirectorySuffix("flutter_view", applicationContext);
        loadNativeLibrary(applicationContext);
        Log.i(TAG, "startInitialization (end)");
    }

    private static void loadNativeLibrary(Context applicationContext) {
        try {
            LibraryLoader.get(LibraryProcessType.PROCESS_BROWSER).ensureInitialized(applicationContext);
            String[] args = new String[0];
            nativeInit(applicationContext, args);
        } catch (ProcessInitException e) {
            Log.e(TAG, "Unable to load Flutter binary.", e);
            throw new RuntimeException(e);
        }
    }

    private static native void nativeInit(Context context, String[] args);
    private static native void nativeRecordStartTimestamp(long initTimeMillis);
}
