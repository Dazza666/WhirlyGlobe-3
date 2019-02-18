/*
 *  QuadImageFrameLoader.h
 *  WhirlyGlobeLib
 *
 *  Created by Steve Gifford on 2/15/19.
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

#import "QuadSamplingController.h"
#import "QuadLoaderReturn.h"

namespace WhirlyKit
{
    
class QuadImageFrameLoader;
    
// We store cancels and adss to do them all at once
// The subclass passes its own version of this around
class QIFBatchOps
{
public:
    QIFBatchOps();
    virtual ~QIFBatchOps();
};

// Assets and status associated with a single tile's frame
class QIFFrameAsset
{
public:
    typedef enum {Empty,Loaded,Loading} State;
    
    QIFFrameAsset();
    virtual ~QIFFrameAsset();
    
    // What the frame is doing
    State getState();
    
    // Load priority
    int getPriority();
    
    // Texture ID (if loaded)
    SimpleIdentity getTexID();
    
    // Clear out the texture and reset
    virtual void clear(QuadImageFrameLoader *loader,QIFBatchOps *batchOps,ChangeSet &changes);

    // Update priority for an existing fetch request
    virtual bool updateFetching(QuadImageFrameLoader *loader,int newPriority,double newImportance);

    // Cancel an outstanding fetch
    virtual void cancelFetch(QuadImageFrameLoader *loader,QIFBatchOps *batchOps);

    // Keep track of the texture ID
    virtual void loadSuccess(QuadImageFrameLoader *loader,Texture *tex);
    
    // Clear out state
    virtual void loadFailed(QuadImageFrameLoader *loader);
    
protected:
    State state;
    
    int priority;
    double importance;
    
    // If set, the texture ID for this asset
    SimpleIdentity texID;
};

typedef std::shared_ptr<QIFFrameAsset> QIFFrameAssetRef;

// Holds all the frame assets and manages various loading state
class QIFTileAsset
{
public:
    QIFTileAsset(const QuadTreeNew::ImportantNode &ident, int numFrames);
    virtual ~QIFTileAsset();
    
    typedef enum {Waiting,Active} State;
    
    State getState();
    
    bool getShouldEnable();
    void setShouldEnable(bool newVal);
    
    QuadTreeNew::ImportantNode getIdent();
    
    const std::vector<SimpleIdentity> &getInstanceDrawIDs();
    
    QIFFrameAssetRef getFrame(int frameID);
    
    // True if any of the frames are in the process of loading
    bool anyFramesLoading();
    
    // True if any frames have loaded
    bool anyFramesLoaded();
    
    // True if the given frame is loading
    bool isFrameLoading(int which);
    
    // Importance value changed, so update the fetcher
    virtual void setImportance(QuadImageFrameLoader *loader,double import);
    
    // Clear out the individual frames, loads and all
    virtual void clearFrames(QuadImageFrameLoader *loader,QIFBatchOps *batchOps,ChangeSet &changes);
    
    // Clear out geometry and all the frame info
    virtual void clear(QuadImageFrameLoader *loader,QIFBatchOps *batchOps, ChangeSet &changes);
    
    // Start fetching data for this tile
    virtual void startFetching(QuadImageFrameLoader *loader,QIFBatchOps *batchOps);

    // Set up the geometry for this tile
    virtual void setupContents(QuadImageFrameLoader *loader,LoadedTileNewRef loadedTile,int defaultDrawPriority,SimpleIdentity shaderID,ChangeSet &changes);
    
    // Cancel any outstanding fetches
    virtual  void cancelFetches(QuadImageFrameLoader *loader,QIFBatchOps *batchOps);
    
    // A single frame loaded successfully
    virtual void frameLoaded(QuadImageFrameLoader *loader,QuadLoaderReturn *loadReturn,Texture *tex,ChangeSet &changes);
    
    // A single frame failed to load
    virtual void frameFailed(QuadImageFrameLoader *loader,QuadLoaderReturn *loadReturn,ChangeSet &changes);
    
protected:
    State state;
    QuadTreeNew::ImportantNode ident;
    
    // Set if the sampling layer thinks this should be on
    bool shouldEnable;
    
    std::vector<SimpleIdentity> instanceDrawIDs;
    
    std::vector<QIFFrameAssetRef> frames;
    
    int drawPriority;
};

typedef std::shared_ptr<QIFTileAsset> QIFTileAssetRef;
typedef std::map<QuadTreeNew::Node,QIFTileAssetRef> QIFTileAssetMap;

// Information about a single tile and its current state
class QIFTileState
{
public:
    QIFTileState(int numFrames);
    
    QuadTreeNew::Node node;
    
    // Set if this should be enabled
    bool enable;
    
    // The geometry used to represent the tile
    std::vector<SimpleIdentity> instanceDrawIDs;
    
    // Information about each frame
    class FrameInfo {
    public:
        FrameInfo();
        
        // Node we're using a texture from (could be this one)
        QuadTreeNew::Node texNode;
        SimpleIdentity texID;
    };
    
    // A texture ID per frame
    std::vector<FrameInfo> frames;
};
typedef std::shared_ptr<QIFTileState> QIFTileStateRef;
    
// Used to track loading state and hand it over to the main thread
class QIFRenderState
{
public:
    QIFRenderState();
    QIFRenderState(int numFrames);
    
    std::map<QuadTreeNew::Node,QIFTileStateRef> tiles;
    
    // Number of tiles loaded for each frame
    std::vector<int> tilesLoaded;
    // Number of tiles at the lowest level loaded for each frame
    std::vector<bool> topTilesLoaded;
    
    bool hasUpdate(double curFrame);
    
    double lastCurFrame;
    TimeInterval lastRenderTime;
    TimeInterval lastUpdate;
    
    // Update what the scene is looking at.  Ideally not every frame.
    void updateScene(Scene *scene,double curFrame,TimeInterval now,bool flipY,const RGBAColor &color,ChangeSet &changes);
};

/** Quad Image Frame Loader
 
    This quad loader handles the logic for a multi-frame set of images.
  */
class QuadImageFrameLoader : public QuadTileBuilderDelegate
{
public:
    QuadImageFrameLoader();
    virtual ~QuadImageFrameLoader();
    
    /// Called when the builder first starts up.  Keep this around if you need it.
    virtual void setBuilder(QuadTileBuilder *builder,QuadDisplayControllerNew *control);
    
    /// Before we tell the delegate to unload tiles, see if they want to keep them around
    /// Returns the tiles we want to preserve after all
    virtual QuadTreeNew::NodeSet builderUnloadCheck(QuadTileBuilder *builder,
                                                    const WhirlyKit::QuadTreeNew::ImportantNodeSet &loadTiles,
                                                    const WhirlyKit::QuadTreeNew::NodeSet &unloadTiles,
                                                    int targetLevel);
    
    /// Load the given group of tiles.  If you don't load them immediately, up to you to cancel any requests
    virtual void builderLoad(QuadTileBuilder *builder,
                             const WhirlyKit::TileBuilderDelegateInfo &updates,
                             ChangeSet &changes);
    
    /// Called right before the layer thread flushes all its current changes
    virtual void builderPreSceneFlush(QuadTileBuilder *builder,ChangeSet &changes);
    
    /// Shutdown called on the layer thread if you stuff to clean up
    virtual void builderShutdown(QuadTileBuilder *builder,ChangeSet &changes);
    
    /// Color for polygons created during loading
    void setColor(RGBAColor &inColor);
    const RGBAColor &getColor();
    
    /// Render target for the geometry being created
    void setRenderTarget(SimpleIdentity renderTargetID);
    SimpleIdentity getRenderTarget();

protected:
    // Construct a platform specific tile asset in the subclass
    virtual QIFTileAssetRef makeTileAsset() = 0;
    virtual QIFTileAssetRef addNewTile(const QuadTreeNew::ImportantNode &ident,QIFBatchOps *batchOps,ChangeSet &changes);
    
    RGBAColor color;
    SimpleIdentity renderTargetID;
};

}