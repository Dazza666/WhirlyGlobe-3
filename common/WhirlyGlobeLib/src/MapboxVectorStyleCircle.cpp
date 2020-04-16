/*
*  MapboxVectorStyleCircle.h
*  WhirlyGlobe-MaplyComponent
*
*  Created by Steve Gifford on 2/17/15.
*  Copyright 2011-2015 mousebird consulting
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

#import "MapboxVectorStyleCircle.h"
#import "WhirlyKitLog.h"

namespace WhirlyKit
{

bool MapboxVectorCirclePaint::parse(MapboxVectorStyleSetImplRef styleSet,DictionaryRef styleEntry)
{
    if (!styleSet)
        return false;
    
    radius = styleSet->doubleValue("circle-radius",styleEntry,5.0);
    fillColor = styleSet->colorValue("circle-color",NULL,styleEntry,RGBAColor::white(),false);
    opacity = styleSet->doubleValue("circle-opacity",styleEntry,1.0);
    strokeWidth = styleSet->doubleValue("circle-stroke-width",styleEntry,0.0);
    strokeColor = styleSet->colorValue("circle-stroke-color",NULL,styleEntry,RGBAColor::black(),false);
    strokeOpacity = styleSet->doubleValue("circle-stroke-opacity",styleEntry,1.0);
    
    return true;
}

bool MapboxVectorLayerCircle::parse(MapboxVectorStyleSetImplRef styleSet,
                                    DictionaryRef styleEntry,
                                    MapboxVectorStyleLayerRef refLayer,
                                    int drawPriority)
{
    if (!MapboxVectorStyleLayer::parse(styleSet,styleEntry,refLayer,drawPriority) ||
        !paint.parse(styleSet, styleEntry->getDict("paint")))
        return false;
    
//    // We want the texture a bit bigger than specified
//    float scale = styleSet.tileStyleSettings.markerScale * 2;
//
//    // Build an image for the circle
//    float buffer = 1.0 * scale;
//    float radius = _paint.radius*scale;
//    float strokeWidth = _paint.strokeWidth*scale;
//    float size = ceil(buffer + radius + strokeWidth)*2;
//    circleSize = CGSizeMake(size / 2, size / 2);
//    UIGraphicsBeginImageContext(CGSizeMake(size, size));
//    // TODO: Use the opacity
//    [[UIColor clearColor] setFill];
//    CGContextRef ctx = UIGraphicsGetCurrentContext();
//    CGContextFillRect(ctx, CGRectMake(0.0, 0.0, size, size));
//
//    // Outer stroke
//    if (strokeWidth > 0.0) {
//        CGContextBeginPath(ctx);
//        CGContextAddEllipseInRect(ctx, CGRectMake(size/2.0-radius-strokeWidth, size/2.0-radius-strokeWidth, 2*(radius+strokeWidth), 2*(radius+strokeWidth)));
//        [_paint.strokeColor setFill];
//        CGContextDrawPath(ctx, kCGPathFill);
//    }
//
//    // Inner circle
//    CGContextBeginPath(ctx);
//    CGContextAddEllipseInRect(ctx, CGRectMake(size/2.0-radius, size/2.0-radius, 2*radius, 2*radius));
//    [_paint.fillColor setFill];
//    CGContextDrawPath(ctx, kCGPathFill);
//
//    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
//    UIGraphicsEndImageContext();
    
    RGBAColor theFillColor = (*paint.fillColor) * paint.opacity;
    RGBAColor theStrokeColor = (*paint.strokeColor) * paint.strokeOpacity;
    circleTexID = styleSet->makeCircleTexture(paint.radius,theFillColor,theStrokeColor,&circleSize);

    // Larger circles are slightly more important
    importance = drawPriority/1000 + styleSet->tileStyleSettings->markerImportance + paint.radius / 100000.0;

    uuidField = styleSet->tileStyleSettings->uuidField;
    
    return true;
}

void MapboxVectorLayerCircle::cleanup(ChangeSet &changes)
{
    if (circleTexID != EmptyIdentity)
        changes.push_back(new RemTextureReq(circleTexID));
}

void MapboxVectorLayerCircle::buildObjects(std::vector<VectorObjectRef> &vecObjs,
                                          VectorTileDataRef tileInfo)
{
    if (!visible)
        return;
    
    ComponentObjectRef compObj(new ComponentObject());
    
    // Default settings
    MarkerInfo markerInfo(true);
    markerInfo.drawPriority = drawPriority;
    markerInfo.programID = styleSet->screenMarkerProgramID;
    
    // Need to find all the points, way down deep
    std::vector<WhirlyKit::Marker *> markers;
    for (auto vecObj : vecObjs) {
        if (vecObj->getVectorType() == VectorPointType) {
            for (VectorShapeRef shape : vecObj->shapes) {
                VectorPointsRef pts = std::dynamic_pointer_cast<VectorPoints>(shape);
                if (pts) {
                    for (auto pt : pts->pts) {
                        // Add a marker per point
                        WhirlyKit::Marker *marker = new WhirlyKit::Marker();
                        marker->loc = GeoCoord(pt.x(),pt.y());
                        marker->texIDs.push_back(circleTexID);
                        marker->width = circleSize.x(); marker->height = circleSize.y();
                        marker->layoutWidth = circleSize.x(); marker->layoutHeight = circleSize.y();
                        marker->layoutImportance = importance + (101-tileInfo->ident.level)/100.0;
                        if (selectable) {
                            marker->isSelectable = true;
                            marker->selectID = Identifiable::genId();
                            // TODO: Figure out selection
//                            styleSet->compManage->addSelectObject(marker->SelectID,marker);
                            compObj->selectIDs.insert(marker->selectID);
                        }
                        if (!uuidField.empty())
                            marker->uniqueID = uuidField;
                        markers.push_back(marker);
                    }
                }
            }
        }
    }
    
    // Set up the markers and get a change set
    SimpleIdentity markerID = styleSet->markerManage->addMarkers(markers, markerInfo, tileInfo->changes);
    if (markerID != EmptyIdentity)
        compObj->markerIDs.insert(markerID);
    styleSet->compManage->addComponentObject(compObj);
    tileInfo->compObjs.push_back(compObj);
}

}