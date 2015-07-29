//
//  Ball.cpp
//  kinectTest
//
//  Created by Ryota Niinomi on 2015/07/20.
//
//

#include "Ball.h"

Ball::Ball(ofVec3f vec) {
    x = vec.x;
    y = vec.y;
    radius = 5;
    speed = 5;
}

void Ball::update() {
    if (x < 0 || x > ofGetWidth()) {
        dirX *= -1;
    }
    if (y < 0 || y > ofGetHeight()) {
        dirY *= -1;
    }
    x += (speed * dirX);
    y += (speed * dirY);
}

void Ball::draw() {
    ofSetColor(255, 0, 0);
//    ofCircle(x, y, radius);
    ofCircle(getPosition().x, getPosition().y, getRadius());
}

void Ball::changeDir() {
    dirX *= -1;
}