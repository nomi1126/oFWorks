#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxTriangle.h"
#include "ofxOpenCv.h"

#define CAM_WIDTH      640
#define CAM_HEIGHT     480

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
private:
    ofVideoGrabber          vidGrabber;
    ofxCvColorImage         colorImage;
    ofxCvGrayscaleImage     threImg;
    ofxCvGrayscaleImage     bgImg;
    
    ofxCvContourFinder      contourFinder;
    ofxTriangle             triangle;
    
    int                     threshold;
    bool                    bLearnBackground;
};
