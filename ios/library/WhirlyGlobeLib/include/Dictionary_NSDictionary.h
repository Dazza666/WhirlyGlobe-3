/*
 *  Dictionary_NSDictionary.h
 *  WhirlyGlobeLib
 *
 *  Created by Steve Gifford on 1/24/19.
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

#import <Foundation/Foundation.h>
#import "Dictionary.h"

namespace WhirlyKit
{
    
/// The Dictionary is my cross platform replacement for NSDictionary
/// On iOS its just a wrapper
class iosDictionary : public Dictionary
{
public:
    iosDictionary();
    iosDictionary(NSDictionary *dict);
    // Copy constructor
    iosDictionary(const iosDictionary &that);
    virtual ~iosDictionary();
    
    /// Returns true if the field exists
    bool hasField(const std::string &name) const;
    
    /// Returns the field type
    DictionaryType getType(const std::string &name) const;
    
    /// Return an int, using the default if it's missing
    int getInt(const std::string &name,int defVal=0.0) const;
    /// Return a 64 bit unique identity or 0 if missing
    SimpleIdentity getIdentity(const std::string &name) const;
    /// Interpret an int as a boolean
    bool getBool(const std::string &name,bool defVal=false) const;
    /// Interpret an int as a RGBA color
    RGBAColor getColor(const std::string &name,const RGBAColor &defVal) const;
    /// Return a double, using the default if it's missing
    double getDouble(const std::string &name,double defVal=0.0) const;
    /// Return a string, or empty if it's missing
    std::string getString(const std::string &name) const;
    /// Return a string, using the default if it's missing
    std::string getString(const std::string &name,const std::string &defVal) const;
    
public:
    NSDictionary *dict;
};

/// This version of the dictionary can be modified
class iosMutableDictionary : public MutableDictionary
{
public:
    iosMutableDictionary();
    iosMutableDictionary(NSMutableDictionary *dict);
    iosMutableDictionary(MutableDictionaryRef dict);
    // Assignment operator
    virtual iosMutableDictionary &operator = (const iosMutableDictionary &that);
    virtual ~iosMutableDictionary();
    virtual MutableDictionaryRef copy();

    /// Returns true if the field exists
    bool hasField(const std::string &name) const;
    
    /// Returns the field type
    DictionaryType getType(const std::string &name) const;
    
    /// Return an int, using the default if it's missing
    int getInt(const std::string &name,int defVal=0.0) const;
    /// Return a 64 bit unique identity or 0 if missing
    SimpleIdentity getIdentity(const std::string &name) const;
    /// Interpret an int as a boolean
    bool getBool(const std::string &name,bool defVal=false) const;
    /// Interpret an int as a RGBA color
    RGBAColor getColor(const std::string &name,const RGBAColor &defVal) const;
    /// Return a double, using the default if it's missing
    double getDouble(const std::string &name,double defVal=0.0) const;
    /// Return a string, or empty if it's missing
    std::string getString(const std::string &name) const;
    /// Return a string, using the default if it's missing
    std::string getString(const std::string &name,const std::string &defVal) const;

    /// Clean out the contents
    void clear();
    
    /// Remove the given field by name
    void removeField(const std::string &name);
    
    /// Set field as int
    void setInt(const std::string &name,int val);
    /// Set field as 64 bit unique value
    void setIdentifiable(const std::string &name,SimpleIdentity val);
    /// Set field as double
    void setDouble(const std::string &name,double val);
    /// Set field as string
    void setString(const std::string &name,const std::string &val);
    
    // Merge in key-value pairs from another dictionary
    void addEntries(const Dictionary *other);
    
public:
    NSMutableDictionary *dict;
};
    
typedef std::shared_ptr<iosMutableDictionary> iosMutableDictionaryRef;
    
}
