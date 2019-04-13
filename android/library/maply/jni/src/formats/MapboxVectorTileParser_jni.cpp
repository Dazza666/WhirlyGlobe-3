/*
 *  MapboxVectorTileParser_jni.cpp
 *  WhirlyGlobeLib
 *
 *  Created by sjg
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

#import <Formats_jni.h>
#import <Geometry_jni.h>
#import <Vectors_jni.h>
#import <Components_jni.h>
#import "com_mousebird_maply_MapboxVectorTileParser.h"

using namespace WhirlyKit;

template<> MapboxVectorTileParserClassInfo *MapboxVectorTileParserClassInfo::classInfoObj = NULL;

JNIEXPORT void JNICALL Java_com_mousebird_maply_MapboxVectorTileParser_nativeInit
        (JNIEnv *env, jclass cls)
{
    MapboxVectorTileParserClassInfo::getClassInfo(env,cls);
}

JNIEXPORT void JNICALL Java_com_mousebird_maply_MapboxVectorTileParser_initialise
(JNIEnv *env, jobject obj)
{
    try
    {
        MapboxVectorTileParser_Android *inst = new MapboxVectorTileParser_Android();
        MapboxVectorTileParserClassInfo::getClassInfo()->setHandle(env,obj,inst);
    }
    catch (...)
    {
        __android_log_print(ANDROID_LOG_VERBOSE, "Maply", "Crash in MapboxVectorTileParser::initialise()");
    }
}

static std::mutex disposeMutex;

JNIEXPORT void JNICALL Java_com_mousebird_maply_MapboxVectorTileParser_dispose
(JNIEnv *env, jobject obj)
{
    try
    {
        MapboxVectorTileParserClassInfo *classInfo = MapboxVectorTileParserClassInfo::getClassInfo();

        {
            std::lock_guard<std::mutex> lock(disposeMutex);
            MapboxVectorTileParser_Android *inst = classInfo->getObject(env,obj);
            if (!inst)
                return;
            delete inst;
        }

        classInfo->clearHandle(env,obj);
    }
    catch (...)
    {
        __android_log_print(ANDROID_LOG_VERBOSE, "Maply", "Crash in MapboxVectorTileParser::dispose()");
    }
}

JNIEXPORT jboolean JNICALL Java_com_mousebird_maply_MapboxVectorTileParser_parseDataNative
        (JNIEnv *env, jobject obj, jbyteArray data, jobject vecTileDataObj)
{
    try
    {
        MapboxVectorTileParser_Android *inst = MapboxVectorTileParserClassInfo::getClassInfo()->getObject(env,obj);
        VectorTileDataRef *tileData = VectorTileDataClassInfo::getClassInfo()->getObject(env,vecTileDataObj);
        if (!obj || !tileData)
            return false;

        // TODO: This has to be associated with the tile data instead.  Otherwise we'll get threading conflicts.
        (*inst).setJavaObject(env,obj);

        // Copy data into a temporary buffer (must we?)
        int len = env->GetArrayLength(data);
        jbyte *rawData = env->GetByteArrayElements(data,NULL);
        RawDataWrapper rawDataWrap(data,len,false);
        bool ret = inst->parse(&rawDataWrap,(*tileData).get());
        if (rawData)
            env->ReleaseByteArrayElements(data,rawData,JNI_ABORT);

        return ret;
    }
    catch (...)
    {
        __android_log_print(ANDROID_LOG_VERBOSE, "Maply", "Crash in MapboxVectorTileParser::parseDataNative()");
    }
}