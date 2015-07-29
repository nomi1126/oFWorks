#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxBox2d.h"
#define MAX_BLOBS 4

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        int winWidth;
        int winHeight;
    	ofxKinect kinect;
    	ofxCvGrayscaleImage grayImage;
    	ofxCvContourFinder contourFinder;
    	int nearThreshold;
    	int farThreshold;
        bool isFullScreen = false;
    
        ofxBox2d world2d;
        vector <ofPtr<ofxBox2dCircle> > circles;
        ofImage snow;
        int numShadows;
        vector<shared_ptr<ofPolyline> > shadowLines;
        vector<shared_ptr<ofxBox2dPolygon> > shadows;
        void makeSnow(float x, float y, float size);
};
