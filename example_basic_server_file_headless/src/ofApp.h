//
// Copyright (c) 2013 Christopher Baker <http://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#pragma once


#include "ofMain.h"
#include "ofxHTTP.h"


class ofApp: public ofBaseApp
{
public:
    void setup();

    // We do not have an draw() method since this is a headless display.

    ofxHTTP::SimpleFileServer server;

};
