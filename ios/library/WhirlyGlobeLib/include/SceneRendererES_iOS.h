/*
 *  SceneRenderereES_iOS.h
 *  WhirlyGlobeLib
 *
 *  Created by Steve Gifford on 1/28/19.
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

#import <UIKit/UIKit.h>
#import "SceneRendererES2.h"

namespace WhirlyKit {

/** SceneRenderer - iOS version
 
    This allocates and manages the context.
  */
class SceneRendererES_iOS : public SceneRendererES2 {
public:
    SceneRendererES_iOS();

    /// Set the current OpenGL ES context if there is one
    void useContext();
    
    /// Return the associated context
    EAGLContext *getContext();
    
protected:
    EAGLContext *context;
};
    
typedef std::shared_ptr<SceneRendererES_iOS> SceneRendererES_iOSRef;
    
}
