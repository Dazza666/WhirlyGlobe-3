/*
 *  MapboxVectorTileParser.h
 *  WhirlyGlobeLib
 *
 *  Created by Steve Gifford on 5/25/16.
 *  Copyright 2011-2016 mousebird consulting
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

#import "vector_tile.pb.h"
#import "VectorObject.h"
#import "QuadTreeNew.h"
#import "ImageTile.h"
#import "ComponentManager.h"

namespace WhirlyKit
{

typedef enum {
    GeomTypeUnknown = 0,
    GeomTypePoint = 1,
    GeomTypeLineString = 2,
    GeomTypePolygon = 3
} MapnikGeometryType;

typedef enum {
    SEG_END    = 0,
    SEG_MOVETO = 1,
    SEG_LINETO = 2,
    SEG_CLOSE = (0x40 | 0x0f)
} MapnikCommandType;

/**
 Information about a single vector tile being parsed.  This is passed into the buildObjects:
 method of a MaplyVectorStyle.
 <br>
 Also a container for the data added for a tile.
 */
class VectorTileData
{
public:
    VectorTileData();
    // Construct by just taking the outline information.  No data.
    VectorTileData(const VectorTileData &);
    virtual ~VectorTileData();
    
    /// Merge the contents of the other one
    void mergeFrom(VectorTileData *);
    
    /// Clear out any added objects (but not ident, bounds)
    void clear();
    
    /// Tile ID for this tile
    QuadTreeNew::Node ident;
    
    /// Bounding box in local coordinates
    MbrD bbox;
    
    /// Bounding box in geographic
    MbrD geoBBox;

    /// Component objects already added to the display, but not yet visible.
    std::vector<ComponentObjectRef> compObjs;
    
    /// If there were any raster layers, they're here by name
    std::vector<ImageTileRef> images;
    
    /// If we asked to preserve the vector objects, these are them
    std::vector<VectorObjectRef> vecObjs;
    
    /// These are vector objects sorted by the Style IDs that will build them
    std::map<SimpleIdentity,std::vector<VectorObjectRef> *> vecObjsByStyle;

    /// If there are any wkcategory tags, we'll sort the component objects into groups
    std::map<std::string,std::vector<ComponentObjectRef> > categories;
};
    
typedef std::shared_ptr<VectorTileData> VectorTileDataRef;
    
/** This object parses the data in Mapbox Vector Tile format.
  */
class MapboxVectorTileParser
{
public:
    MapboxVectorTileParser();
    ~MapboxVectorTileParser();
    
    /// If set, we'll parse into local coordinates as specified by the bounding box, rather than geo coords
    bool localCoords;
    
    /// Keep the vector objects around as we parse them
    bool keepVectors;
    
    /// Parse everything, even if there's no style for it
    bool parseAll;
    
    // Add a category for a particulary style ID
    // These are used for sorting later on
    void addCategory(const std::string &category,long long styleID);

    // Make a platform specific copy of the tile data
    virtual VectorTileDataRef makeTileDataCopy(VectorTileData *inTileData) = 0;

    // Subclass can fill this in to check if a layer has any styles
    virtual bool layerShouldParse(const std::string &layerName,VectorTileData *tileData);
    
    // Subclass returns the style IDs that get a shot at the given feature
    virtual SimpleIDSet stylesForFeature(MutableDictionaryRef attributes,const std::string &layerName,VectorTileData *tileData) = 0;

    // Parse the vector tile and return a list of vectors.
    // Returns false on failure.
    virtual bool parse(RawData *rawData,VectorTileData *tileData);
    
    // The subclass calls the appropriate style to build component objects
    //  which are then returned in the VectorTileData
    virtual void buildForStyle(long long styleID,std::vector<VectorObjectRef> &vecObjs,VectorTileDataRef data) = 0;
        
protected:
    std::map<long long,std::string> styleCategories;
};

}
