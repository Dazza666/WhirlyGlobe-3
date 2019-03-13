/*
 *  MapboxVectorTilesImageDelegate.h
 *  WhirlyGlobe-MaplyComponent
 *
 *  Created by Steve Gifford on January 24 2018
 *  Copyright 2011-2019 Saildrone
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


#import <Foundation/Foundation.h>
#import "MaplyTileSourceNew.h"
#import "MaplyCoordinate.h"
#import "MaplyVectorStyle.h"
#import "MapboxVectorTiles.h"
#import "MaplyQuadImageFrameLoader.h"

@class MapboxVectorStyleSet;

/**
 An interpreter for Mapbox Vector Tiles.
 
 This will turn vector tiles into images, visual objects, or a combination of the two.
 */
@interface MapboxVectorInterpreter : NSObject<MaplyLoaderInterpreter>

/** This version of the init takes an image style set, a vector style set,
    and an offline renderer to build the image tiles.
 
    Image tiles will be used as a background and vectors put on top of them.
    This is very nice for the globe, but requires specialized style sheets.
  */
- (instancetype _Nullable ) initWithImageStyle:(MapboxVectorStyleSet *__nonnull)imageStyle
                         offlineRender:(MaplyRenderController *__nonnull)renderControl
                           vectorStyle:(NSObject<MaplyVectorStyleDelegate> *__nonnull)vectorStyle
                                 viewC:(MaplyBaseViewController *__nonnull)viewC;

/** This version of the init builds visual features for vector tiles.
 
    This interpreter can be used as overlay data or a full map, depending
    on how your style is configured.
  */
- (instancetype __nullable) initWithVectorStyle:(NSObject<MaplyVectorStyleDelegate> *__nonnull)vectorStyle
                                          viewC:(MaplyBaseViewController *__nonnull)viewC;

@end