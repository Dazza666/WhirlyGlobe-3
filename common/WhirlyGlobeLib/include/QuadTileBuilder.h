/*
 *  QuadTileBuilder.h
 *  WhirlyGlobeLib
 *
 *  Created by Steve Gifford on 3/29/18.
 *  Copyright 2012-2018 Saildrone Inc
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

#import "QuadDisplayControllerNew.h"
#import "LoadedTileNew.h"

namespace WhirlyKit
{

class QuadTileBuilder;
    
/**
    Tile Builder Delegate Info
 
    This is passed to a Tile Builder Delegate when changes are being made in the Tile Builder;
  */
class TileBuilderDelegateInfo {
public:
    int targetLevel;
    LoadedTileVec loadTiles;
    QuadTreeNew::NodeSet unloadTiles;
    LoadedTileVec enableTiles,disableTiles;
    QuadTreeNew::ImportantNodeSet changeTiles;
};

/// Protocol used by the tile builder to notify an interested party about what's
///  loaded.  If, for example, you want to attach textures to things.
class QuadTileBuilderDelegate
{
public:
    QuadTileBuilderDelegate();
    virtual ~QuadTileBuilderDelegate();
    
    /// Called when the builder first starts up.  Keep this around if you need it.
    virtual void builderSet(QuadTileBuilder *builder) = 0;
    
    /// Before we tell the delegate to unload tiles, see if they want to keep them around
    /// Returns the tiles we want to preserve after all
    virtual QuadTreeNew::NodeSet builderUnloadCheck(QuadTileBuilder *builder,
                                                  const WhirlyKit::QuadTreeNew::ImportantNodeSet &loadTiles,
                                                  const WhirlyKit::QuadTreeNew::NodeSet &unloadTiles,
                                                  int targetLevel) = 0;
    
    /// Load the given group of tiles.  If you don't load them immediately, up to you to cancel any requests
    virtual void builderLoad(QuadTileBuilder *builder,
                           const WhirlyKit::TileBuilderDelegateInfo &updates,
                           ChangeSet &changes) = 0;
    
    /// Called right before the layer thread flushes all its current changes
    virtual void builderPreSceneFlush(QuadTileBuilder *builder) = 0;

    /// Shutdown called on the layer thread if you stuff to clean up
    virtual void builderShutdown(QuadTileBuilder *builder) = 0;
};
    
typedef std::shared_ptr<QuadTileBuilderDelegate> QuadTileBuilderDelegateRef;

/** The Quad Tile Builder generates geometric tiles based on
 a quad tree and coordinates an image builder on top of those.
 */
class QuadTileBuilder : public QuadLoaderNew
{
    QuadTileBuilder(CoordSystem *coordSys);
    virtual ~QuadTileBuilder();
    
    // Return a tile, if there is one
    LoadedTileNewRef getLoadedTile(const QuadTreeNew::Node &ident);
    
    // Return all the tiles that should be loaded
    TileBuilderDelegateInfo getLoadingState();

    // Note: Fill in getters/setters
    
protected:
    // Coordinate system we're building the tiles in
    CoordSystem *coordSys;
    
    // If set, we'll cover the poles of a curved coordinate system (e.g. spherical mercator)
    bool coverPoles;
    
    // If set, we'll generate skirts to match between levels of detail
    bool edgeMatching;
    
    // Set the draw priority values for produced tiles
    int baseDrawPriority;
    
    // Offset between levels for a calculated draw priority
    int drawPriorityPerLevel;
    
    // Set if we're using single level loading logic
    bool singleLevel;
    
    // If set, we'll print too much information
    bool debugMode;
    
    // If set, we'll notify the delegate when tiles are loaded and unloaded
    QuadTileBuilderDelegate *delegate;
};
    
}
