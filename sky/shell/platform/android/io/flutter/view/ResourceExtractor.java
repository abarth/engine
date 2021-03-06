// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package io.flutter.view;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.AsyncTask;
import android.util.Log;

import org.chromium.base.PathUtils;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashSet;
import java.util.List;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;

/**
 * A class to intialize the native code.
 **/
class ResourceExtractor {
    private static final String TAG = "ResourceExtractor";
    private static final String TIMESTAMP_PREFIX = "res_timestamp-";

    private class ExtractTask extends AsyncTask<Void, Void, Void> {
        private static final int BUFFER_SIZE = 16 * 1024;

        ExtractTask() { }

        private void extractResources() {
            final File dataDir = new File(PathUtils.getDataDirectory(mContext));

            final String timestamp = checkTimestamp(dataDir);
            if (timestamp != null) {
                deleteFiles();
            }

            final AssetManager manager = mContext.getResources().getAssets();
            try {
                byte[] buffer = null;
                final String[] assets = manager.list("");
                for (String asset : assets) {
                    if (!mResources.contains(asset))
                        continue;
                    final File output = new File(dataDir, asset);
                    if (output.exists())
                        continue;
                    InputStream is = null;
                    OutputStream os = null;
                    try {
                        is = manager.open(asset);
                        os = new FileOutputStream(output);
                        if (buffer == null) {
                            buffer = new byte[BUFFER_SIZE];
                        }

                        int count = 0;
                        while ((count = is.read(buffer, 0, BUFFER_SIZE)) != -1) {
                            os.write(buffer, 0, count);
                        }
                        os.flush();
                    } finally {
                        try {
                            if (is != null) {
                                is.close();
                            }
                        } finally {
                            if (os != null) {
                                os.close();
                            }
                        }
                    }
                }
            } catch (IOException e) {
                Log.w(TAG, "Exception unpacking resources: " + e.getMessage());
                deleteFiles();
                return;
            }

            if (timestamp != null) {
                try {
                    new File(dataDir, timestamp).createNewFile();
                } catch (IOException e) {
                    Log.w(TAG, "Failed to write resource timestamp");
                }
            }
        }

        private String checkTimestamp(File dataDir) {
            PackageManager packageManager = mContext.getPackageManager();
            PackageInfo packageInfo = null;

            try {
                packageInfo = packageManager.getPackageInfo(mContext.getPackageName(), 0);
            } catch (PackageManager.NameNotFoundException e) {
                return TIMESTAMP_PREFIX;
            }

            if (packageInfo == null) {
                return TIMESTAMP_PREFIX;
            }

            String expectedTimestamp =
                    TIMESTAMP_PREFIX + packageInfo.versionCode + "-" + packageInfo.lastUpdateTime;

            final String[] existingTimestamps = getExistingTimestamps(dataDir);
            if (existingTimestamps.length != 1
                    || !expectedTimestamp.equals(existingTimestamps[0])) {
                return expectedTimestamp;
            }

            return null;
        }

        @Override
        protected Void doInBackground(Void... unused) {
            extractResources();
            return null;
        }
    }

    private final Context mContext;
    private final HashSet<String> mResources;
    private ExtractTask mExtractTask;

    ResourceExtractor(Context context) {
        mContext = context;
        mResources = new HashSet<String>();
    }

    void addResources(List<String> resources) {
        mResources.addAll(resources);
    }

    void start() {
        assert mExtractTask == null;
        mExtractTask = new ExtractTask();
        mExtractTask.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
    }

    void waitForCompletion() {
        assert mExtractTask != null;

        try {
            mExtractTask.get();
        } catch (CancellationException e) {
            deleteFiles();
        } catch (ExecutionException e2) {
            deleteFiles();
        } catch (InterruptedException e3) {
            deleteFiles();
        }
    }

    private String[] getExistingTimestamps(File dataDir) {
        return dataDir.list(new FilenameFilter() {
            @Override
            public boolean accept(File dir, String name) {
                return name.startsWith(TIMESTAMP_PREFIX);
            }
        });
    }

    private void deleteFiles() {
        final File dataDir = new File(PathUtils.getDataDirectory(mContext));
        for (String resource : mResources) {
            final File file = new File(dataDir, resource);
            if (file.exists()) {
                file.delete();
            }
        }
        for (String timestamp : getExistingTimestamps(dataDir)) {
            new File(dataDir, timestamp).delete();
        }
    }
}
