//
//  Ball.h
//  kinectTest
//
//  Created by Ryota Niinomi on 2015/07/20.
//
//

//#ifndef __kinectTest__Ball__
//#define __kinectTest__Ball__
//
//#include <stdio.h>
//
//#endif /* defined(__kinectTest__Ball__) */

#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"

class Ball : public ofxBox2dCircle {
private:
    float x;
    float y;
    float speed;
    float radius;
    int dirX = 1;
    int dirY = 1;
public:
    Ball(ofVec3f vec);
    void draw();
    void update();
    void changeDir();
};