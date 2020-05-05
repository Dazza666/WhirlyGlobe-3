/*
 *  MapboxVectorStyleSet_Android.cpp
 *  WhirlyGlobeLib
 *
 *  Created by Steve Gifford on 4/29/20.
 *  Copyright 2011-2020 mousebird consulting
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

#import "MapboxVectorStyleSet_Android.h"
#import <string>
#import <iostream>
#import <sstream>
#import <cstdlib>
#import "Formats_jni.h"
#import "LabelsAndMarkers_jni.h"

namespace WhirlyKit
{

MapboxVectorStyleSetImpl_Android::MapboxVectorStyleSetImpl_Android(Scene *scene,VectorStyleSettingsImplRef settings)
: MapboxVectorStyleSetImpl(scene,settings), thisObj(NULL),
    makeLabelInfoMethod(NULL), makeCircleTextureMethod(NULL), makeLineTextureMethod(NULL)
{
}

void MapboxVectorStyleSetImpl_Android::setupMethods(JNIEnv *env)
{
    // TODO: Turn global reference into local reference for this object

    if (!makeLabelInfoMethod) {
        jclass thisClass = MapboxVectorStyleSetClassInfo::getClassInfo()->getClass();
        // TODO: Get the right signatures
        makeLabelInfoMethod = env->GetMethodID(thisClass, "labelInfoForFont",
                                               "(Ljava/lang/String;F)Lcom/mousebird/maply/LabelInfo;");
//        makeCircleTextureMethod = env->GetMethodID(thisClass, "makeLabelInfo",
//                                                   "");
//        makeLineTextureMethod = env->GetMethodID(thisClass, "makeLabelInfo",
//                                                 "");
    }
}

void MapboxVectorStyleSetImpl_Android::cleanup(JNIEnv *env)
{
    for (auto labelInfo : labelInfos)
        env->DeleteGlobalRef(labelInfo.second->labelInfoObj);
    labelInfos.clear();
}

MapboxVectorStyleSetImpl_Android::~MapboxVectorStyleSetImpl_Android()
{
}

SimpleIdentity MapboxVectorStyleSetImpl_Android::makeCircleTexture(VectorStyleInst *inInst,
        double radius,
        const RGBAColor &fillColor,
        const RGBAColor &strokeColor,
        float strokeWidth,
        Point2f *circleSize)
{
    // TODO: Implement
    return EmptyIdentity;
}

SimpleIdentity MapboxVectorStyleSetImpl_Android::makeLineTexture(VectorStyleInst *inInst,const std::vector<double> &dashComponents)
{
    // TODO: Implement
    return EmptyIdentity;
}

LabelInfoRef MapboxVectorStyleSetImpl_Android::makeLabelInfo(VectorStyleInst *inInst,const std::string &fontName,float fontSize)
{
    VectorStyleInst_Android *inst = (VectorStyleInst_Android *)inInst;
    std::pair<std::string, float> entry(fontName,fontSize);

    LabelInfoAndroid *refLabelInfo = NULL;
    auto it = labelInfos.find(entry);
    if (it != labelInfos.end())
        refLabelInfo = it->second;
    else {
        jstring jFontNameStr = inst->env->NewStringUTF(fontName.c_str());
        jobject labelInfoGlobeObj = inst->env->NewGlobalRef(inst->env->CallObjectMethod(thisObj,makeLabelInfoMethod,jFontNameStr,fontSize));
        inst->env->DeleteLocalRef(jFontNameStr);
        refLabelInfo = LabelInfoClassInfo::getClassInfo()->getObject(inst->env,labelInfoGlobeObj);
        refLabelInfo->labelInfoObj = labelInfoGlobeObj;
        labelInfos[entry] = refLabelInfo;
    }

    // The current labelInfo object needs to point at the active Env for this call
    LabelInfoAndroidRef labelInfo(new LabelInfoAndroid(*refLabelInfo));
    labelInfo->env = inst->env;
    return labelInfo;
}

SingleLabelRef MapboxVectorStyleSetImpl_Android::makeSingleLabel(VectorStyleInst *inInst,const std::string &text)
{
    SingleLabelAndroid *label = new SingleLabelAndroid();

    // Break up the lines and turn the characters into code points for Java
    std::istringstream ss(text);
    std::string line;
    while (std::getline(ss, line, ss.widen('\n'))) {
        std::vector<int> codePoints;
        for (const char *chr = line.c_str();*chr;++chr)
            codePoints.push_back(*chr);
        label->codePointsLines.push_back(codePoints);
    }

    return SingleLabelRef(label);
}

ComponentObjectRef MapboxVectorStyleSetImpl_Android::makeComponentObject(VectorStyleInst *inInst)
{
    return ComponentObjectRef(new ComponentObject());
}

}