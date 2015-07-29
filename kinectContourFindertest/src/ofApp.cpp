#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    winWidth = ofGetScreenWidth();
    winHeight = ofGetScreenHeight();
    ofBackground(0, 0, 0);
    ofSetWindowShape(winWidth, winHeight);
    //  box2d
    world2d.init();
    world2d.setFPS(60.0);
    world2d.setGravity(0, 1);
    //  snow
    snow.loadImage("snow.png");
    snow.setAnchorPercent(0.5, 0.5);
    //  ground (invisible)
    world2d.createGround(100, winHeight - 100, winWidth - 100, winHeight - 100);
    
	kinect.setRegistration(true);
	kinect.init();
    kinect.open();
	grayImage.allocate(kinect.width, kinect.height);
    
    nearThreshold = 600;
    farThreshold = 1300;
    
    for (int i = 0; i < MAX_BLOBS; i++) {
        shadows.push_back(shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon));
    }
}

void ofApp::makeSnow(float x, float y, float size) {
    //  create a new circle
    ofPtr<ofxBox2dCircle> circle = ofPtr<ofxBox2dCircle>(new ofxBox2dCircle);
    //  set attributes to this circle (density, bounce, friction)
    circle.get()->setPhysics(1.0, 0.5, 0.1);
    circle.get()->setup(world2d.getWorld(), x, y, size);
    //  add this circle to "circles" vector
    circles.push_back(circle);
}

bool objectKiller(ofPtr<ofxBox2dBaseShape> shape) {
    float yPos = shape.get()->getPosition().y;
    return (yPos >= 800);
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();
    
    
    if (kinect.isFrameNew()) {
        //	grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        float *depthData = kinect.getDistancePixels();
        unsigned char *shadowData = grayImage.getPixels();
        for (int k = 0; k < kinect.width * kinect.height; k++) {
            shadowData[k] = (nearThreshold < depthData[k] && depthData[k] < farThreshold) ? 255 : 0;
        }
        grayImage.flagImageChanged();
//        grayImage.invert();
		contourFinder.findContours(grayImage, 300, (kinect.width*kinect.height), 4, false);
        
//        numShadows = contourFinder.nBlobs;
//        for (int j = 0; j < MAX_BLOBS; j++) {
////            shadowLines[j].clear();
////            shadows[j]->clear();
//            if (j < numShadows) {
//                //  make a contour
//                for (int k = 0; k < contourFinder.blobs[j].pts.size(); k++) {
//                    shadowLines[j].addVertex(contourFinder.blobs[j].pts[k]);
//                }
////                shadowLines[j].close();
////                shadowLines[j].simplify();
//                //  make a polygon
////                shadows[j]->clear();
////                shadows[j]->addVertexes(shadowLines[j]);
////                shadows[j]->simplify();
//////                shadows[j]->triangulatePoly(30);
////                shadows[j]->setPhysics(1.0, 0.5, 0.1);
////                shadows[j]->create(world2d.getWorld());
//            }
//        }
        
        
        

        for (int i = 0; i < shadows.size(); i++) {
            shadows[i]->clear();
            shadows[i]->destroy();
        }
//        for (int i = 0; i < shadowLines.size(); i++) {
//            shadowLines[i]->close();
//        }
        shadows.clear();
//        shadowLines.clear();
        for (int i = 0; i < contourFinder.nBlobs; i++){
            if (i < 4) {
            ofPolyline shape;
            for (int j = 0; j < contourFinder.blobs[i].pts.size(); j++) {
                shape.addVertex(contourFinder.blobs[i].pts[j]);
            }
            shared_ptr<ofxBox2dPolygon> polyline = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
            polyline->addVertexes(shape);
            polyline->setPhysics(1.0, 0.5, 0.1);
            polyline->triangulatePoly();
            polyline->create(world2d.getWorld());
            shape.clear();
            shadows.push_back(polyline);
            }
        }
        
        
        if (ofRandom(0, 100) < 40)
            makeSnow(ofRandom(-40, 680), -40, ofRandom(5, 10));
        ofRemove(circles, objectKiller);
        world2d.update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if (isFullScreen) {
    	grayImage.draw(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
    	contourFinder.draw(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
    } else {
    	kinect.drawDepth(10, 10, 400, 300);
    	kinect.draw(420, 10, 400, 300);
    	grayImage.draw(10, 320, 400, 300);
    	contourFinder.draw(10, 320, 400, 300);
        
    	stringstream reportStream;
        reportStream << "nearTreashold : " << nearThreshold << endl
        << "farThreashold : " << farThreshold << endl;
    	ofDrawBitmapString(reportStream.str(), 20, 652);
    }
    
    //  draw contours
    ofSetLineWidth(3);
    ofSetColor(140, 140, 200);
    
//    for (int j = 0; j < numShadows; j++) {
//        shadowLines[j].draw();
//    }
    
    for (int i = 0, len = shadows.size(); i < len; i++) {
        shadows[i]->draw();
    }
    
    for (int i = 0; i < circles.size(); i++) {
        ofPoint pos = circles[i].get()->getPosition();
        float size = circles[i].get()->getRadius() * 10.0;
        snow.draw(pos, size, size);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
		case '>':
		case '.':
			farThreshold ++;
//			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
//			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
//			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
//			if (nearThreshold < 0) nearThreshold = 0;
			break;
            
        case ' ':
            isFullScreen = !isFullScreen;
            break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    winWidth = ofGetScreenWidth();
    winHeight = ofGetScreenHeight();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
