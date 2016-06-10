// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package io.flutter.view;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowInsets;

import org.chromium.base.JNINamespace;

/**
 * An Android view containing a Flutter app.
 */
@JNINamespace("flutter")
public class FlutterView extends SurfaceView  {
    private static final String TAG = "Flutter";
    private long mNativePlatformView;
    private final SurfaceHolder.Callback mSurfaceCallback;

    public FlutterView(Context context) {
        this(context, null);
    }

    public FlutterView(Context context, AttributeSet attrs) {
        super(context, attrs);
        attach();
        assert mNativePlatformView != 0;

        mSurfaceCallback = new SurfaceHolder.Callback() {
            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                Log.i(TAG, "surfaceChanged");
            }

            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Log.i(TAG, "surfaceCreated");
                assert mNativePlatformView != 0;
                nativeSurfaceCreated(mNativePlatformView, holder.getSurface());
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.i(TAG, "surfaceCreated");
                assert mNativePlatformView != 0;
                nativeSurfaceDestroyed(mNativePlatformView);
            }
        };
        getHolder().addCallback(mSurfaceCallback);
    }

    public void destroy() {
        getHolder().removeCallback(mSurfaceCallback);
        nativeDetach(mNativePlatformView);
        mNativePlatformView = 0;
    }

    private void attach() {
        mNativePlatformView = nativeAttach(0);
    }

    private static native long nativeAttach(int inputObserverHandle);
    private static native void nativeDetach(long nativeFlutterView);
    private static native void nativeSurfaceCreated(long nativeFlutterView, Surface surface);
    private static native void nativeSurfaceDestroyed(long nativeFlutterView);
}
