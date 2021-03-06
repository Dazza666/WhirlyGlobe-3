/*
 *  LoaderReturn.java
 *  WhirlyGlobeLib
 *
 *  Created by sjg
 *  Copyright 2011-2019 mousebird consulting
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

package com.mousebird.maply;

import java.util.ArrayList;

/**
 *  Passed in to and returned by the Loader Interpreter.
 *
 *  We pass this into the interpreter with the unparsed data.  It parses it and passes that
 *  data back, possibly with an error.
 */
public class LoaderReturn
{
    protected LoaderReturn() {}

    LoaderReturn(QuadLoaderBase loader) {
        initialise(loader);
    }

    /**
     * Set the tile ID at creation.
     */
    public native void setTileID(int tileX,int tileY,int tileLevel);

    /**
     * Set the frame at creation (-1 not acceptable)
     */
    public native void setFrame(int frame);

    /**
     * Tile this data belongs to.
     */
    public TileID getTileID()
    {
        TileID tileID = new TileID();

        int[] tileInfo = getTileIDNative();
        tileID.x = tileInfo[0];
        tileID.y = tileInfo[1];
        tileID.level = tileInfo[2];

        return tileID;
    }

    private native int[] getTileIDNative();

    /**
     * If we're loading multi-frame data, which frame this is.
     * Otherwise -1.
     */
    public native int getFrame();

    private ArrayList<byte[]> tileData = new ArrayList<byte[]>();

    /**
     * Data returned from a tile request.  Unparsed.
     * You can add multiple of these, but the interpreter should be expecting that.
     */
    public void addTileData(byte[] data)
    {
        tileData.add(data);
    }

    /**
     * Return the tile data objects as an array
     */
    public byte[][] getTileData()
    {
        return tileData.toArray(new byte[0][]);
    }

    /**
     * Return the first data object.  You're probably only expecting the one.
     */
    public byte[] getFirstData()
    {
        if (tileData.isEmpty())
            return null;
        return tileData.get(0);
    }

    /**
     * If set, some part of the parser is letting us know about an error.
     */
    String errorString = null;

    public void finalize()
    {
        dispose();
    }

    static
    {
        nativeInit();
    }
    private static native void nativeInit();
    native void initialise(QuadLoaderBase loader);
    native void dispose();
    private long nativeHandle;
}
