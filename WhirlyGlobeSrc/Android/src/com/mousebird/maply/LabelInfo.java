/*
 *  LabelInfo.java
 *  WhirlyGlobeLib
 *
 *  Created by Steve Gifford on 6/2/14.
 *  Copyright 2011-2014 mousebird consulting
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

import android.graphics.Typeface;

/**
 * We use this class to designate attribute shared by a group of labels.
 * Rather than specifying them individually they all go here.  You would
 * use this class in the course of an addLabels() or addScreenLabels()
 * call on the MaplyController.
 * 
 * @author sjg
 *
 */
public class LabelInfo extends BaseInfo
{
	public LabelInfo()
	{
		initialise();
		setTextColor(1.f,1.f,1.f,1.f);
		setBackgroundColor(0.f,0.f,0.f,0.f);
		setFontSize(48.f);
		setLayoutImportance(Float.MAX_VALUE);
	}
	
	public void finalize()
	{
		dispose();
	}

	/**
	 * Set the text color as float values from 0.0 to 1.0
	 * 
	 * @param r red
	 * @param g green
	 * @param b blue
	 * @param a alpha
	 */
	public native void setTextColor(float r,float g,float b,float a);
	
	/**
	 * Set the background color of text.  This is what appears in the rectangles behind the text.
	 * Color components range from 0.0 to 1.0.
	 * 
	 * @param r red
	 * @param g green
	 * @param b blue
	 * @param a alpha
	 */
	public native void setBackgroundColor(float r,float g,float b,float a);
	
	/**
	 * Set the typeface used in the text.
	 */
	public native void setTypeface(Typeface typeface);

	/**
	 * Set the font size for the text.  For screen labels this controls the geometry size as well.
	 */
	public native void setFontSize(float size);

	/**
	 * The layout engine controls how text is displayed.  It tries to avoid overlaps
	 * and takes priority into account.  The layout importance controls which labels
	 * (or other features) are laid out first.  Bigger is more important.
	 * <p>
	 * Defaults to MAXFLOAT, which is off.  That means the layout engine does not control
	 * the associated labels.
	 */
	public native void setLayoutImportance(float importance);
	
	/**
	 * Return the typeface used for the labels.
	 */
	public native Typeface getTypeface();
	
	/**
	 * Return the text color in a form suitable for Android.
	 */
	public native int getTextColor();

	/**
	 * Return the background text color in a form suitable for Android.
	 */
	public native int getBackColor();

	static
	{
		nativeInit();
	}
	private static native void nativeInit();
	native void initialise();
	native void dispose();
	private long nativeHandle;

}
