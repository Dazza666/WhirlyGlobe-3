/*
 *  SLDNamedLayer.java
 *  WhirlyGlobeLib
 *
 *  Created by Ranen Ghosh on 3/14/17.
 *  Copyright 2011-2017 mousebird consulting
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
package com.mousebird.maply.sld.sldstyleset;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.ArrayList;

import android.util.Log;


import com.mousebird.maply.sld.sldstyleset.SLDUserStyle;
import com.mousebird.maply.sld.sldstyleset.SLDParseHelper;

public class SLDNamedLayer {

    private String name;
    private List<SLDUserStyle> userStyles;

    public String getName() {
        return this.name;
    }

    public SLDNamedLayer(XmlPullParser xpp) throws XmlPullParserException, IOException {
        this.name = "";
        this.userStyles = new ArrayList<SLDUserStyle>();
        while (xpp.next() != XmlPullParser.END_TAG) {
            if (xpp.getEventType() != XmlPullParser.START_TAG) {
                continue;
            }
            Log.i("SLDNamedLayer", xpp.getName());
            if (xpp.getName().equals("Name"))
                this.setName(xpp);
            else if (xpp.getName().equals("UserStyle"))
                this.userStyles.add(new SLDUserStyle(xpp));
            else
                SLDParseHelper.skip(xpp);

        }

    }

    private void setName(XmlPullParser xpp) throws XmlPullParserException, IOException {
        while (xpp.next() != XmlPullParser.END_TAG) {
            if (xpp.getEventType() != XmlPullParser.TEXT) {
                continue;
            }
            this.name = xpp.getText();
        }
    }


}
