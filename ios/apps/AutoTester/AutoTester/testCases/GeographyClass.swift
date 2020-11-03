//
//  GeographyClass.swift
//  AutoTester
//
//  Created by Steve Gifford on 2/12/20.
//  Copyright © 2015-2020 mousebird consulting.
//

import UIKit

public class GeographyClassTestCase: MaplyTestCase {

    override init() {
        super.init()

        self.name = "Geography Class"
        self.implementations = [.globe, .map]
    }
    
    typealias ImageLayer = (loader: MaplyQuadImageLoader, fetcher: MaplyMBTileFetcher)
    var layers : [ImageLayer] = []
    var varTarget : MaplyVariableTarget? = nil
    
    func setupMBTiles(_ name: String, offscreen: Bool, transparent: Bool, drawPriority: Int32, viewC: MaplyBaseViewController) -> ImageLayer? {
        guard let fetcher = MaplyMBTileFetcher(mbTiles: name) else {
            return nil
        }
        
        let params = MaplySamplingParams()
        if (transparent) {
            params.forceMinLevel = false
        }
        params.minZoom = 0
        params.maxZoom = fetcher.maxZoom()
        params.coordSys = MaplySphericalMercator(webStandard: ())
        params.singleLevel = true
        params.coverPoles = true
        params.edgeMatching = true
        params.forceMinLevel = true
        
//        sampleParams.coverPoles = true
//        sampleParams.edgeMatching = true
//        sampleParams.minZoom = tileInfo.minZoom()
//        sampleParams.maxZoom = tileInfo.maxZoom()
//        sampleParams.singleLevel = true
//        sampleParams.minImportance = 1024.0 * 1024.0 / 2.0
        
        guard let loader = MaplyQuadImageLoader(params: params, tileInfo: fetcher.tileInfo(), viewC: viewC) else {
            return nil
        }
        loader.setTileFetcher(fetcher)
        loader.baseDrawPriority = drawPriority
        loader.drawPriorityPerLevel = 1;
        
        let layer = ImageLayer(loader: loader, fetcher: fetcher)

        // For offscreen rendering, we need a target
        if offscreen {
            if varTarget == nil {
                let target = MaplyVariableTarget(type: .imageIntRGBA, viewC: viewC)
                target.setScale(1.0)
                target.drawPriority = drawPriority
                target.buildRectangle = true
                varTarget = target
            }
            loader.setRenderTarget(varTarget!.renderTarget)
        }
        
        return layer
    }
            
    func setupLayers(_ viewC: MaplyBaseViewController) {
        //viewC.clearColor = UIColor .blue;
        if let layer = setupMBTiles("zoom0",
                                    offscreen: false,
                                    transparent: false,
                                    drawPriority: kMaplyImageLayerDrawPriorityDefault,
                                    viewC: viewC) {
            layers.append(layer)
        }
        
        //Another layer over the base map
        guard let fetcher = MaplyMBTileFetcher(mbTiles: "dover_detail") else {
            return
        }
        
        //Set up the parameters
        let params = MaplySamplingParams()
        params.minZoom = 0
        params.maxZoom = fetcher.maxZoom()
        params.coordSys = MaplySphericalMercator(webStandard: ())
        params.singleLevel = true
        params.coverPoles = true
        params.edgeMatching = true
        
        //Create a loader
        guard let loader = MaplyQuadImageLoader(params: params, tileInfo: fetcher.tileInfo(), viewC: viewC) else {
            return
        }
        loader.setTileFetcher(fetcher)
        loader.baseDrawPriority = kMaplyMaxDrawPriorityDefault+1000;
        
        //Wrap them together
        let layer2 = ImageLayer(loader: loader, fetcher: fetcher)
        //Append to the array
        layers.append(layer2)
        
    }
    
    public override func stop() {
        for layer in layers {
            layer.loader.shutdown()
        }
        layers = []
        if let target = varTarget {
            target.shutdown()
        }
        varTarget = nil
    }
    
    public override func setUpWithGlobe(_ globeVC: WhirlyGlobeViewController) {
        setupLayers(globeVC)
        globeVC.animate(toPosition: MaplyCoordinateMakeWithDegrees(1.324502, 51.121632), time: 0.0)
        
        //globeVC.animate(toPosition: MaplyCoordinateMakeWithDegrees(1.324502, 51.121632), height: 0.01, heading: 0, time: 1)
        //globeVC.height(forMapScale: 10)
//        animateToPosition:(MaplyCoordinate)newPos height:(float)newHeight heading:(float)newHeading time:(NSTimeInterval)howLong;
        //globeVC.keepNorthUp = true
        //globeVC.height = 0.0001;
        let catto = globeVC.getZoomLimitsMax()
        let catto2 = globeVC.getZoomLimitsMin()
        let hi = 5;
    }

    public override func setUpWithMap(_ mapVC: MaplyViewController) {
        setupLayers(mapVC)
        //mapVC.animate(toPosition: MaplyCoordinateMakeWithDegrees(-70.101479, -54.650212), time: 1.0)
        mapVC.animate(toPosition: MaplyCoordinateMakeWithDegrees(1.324502, 51.121632), height: 1.01, heading: 0, time: 10)
        //mapVC.height = 12
    }

}
