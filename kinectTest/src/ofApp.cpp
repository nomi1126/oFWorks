#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
    
    //Box2D初期設定
    box2d.init(); //Box2Dの世界を初期化
    box2d.setGravity(0, 1); //重力を設定、下に5の力
    box2d.setFPS(60); //30fpsで表示
    box2d.checkBounds(true);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	// print the intrinsic IR sensor values
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
    nearDepthThreshold = 838;
    farDepthThreshold = 1219;
    scale = 1100;
    translateX = 0;
    translateY = 0;
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
    
    for(int x = 0; x < 300; x++) {
        Ball* ball = new Ball(ofVec3f(ofRandom(ofGetWidth()), ofRandom(ofGetHeight())));
        ball->setPhysics(1.0, 0.8, 0.0);
        ball->setup(box2d.getWorld(), ofRandom(colorImg.width), ofRandom(colorImg.height), 3);
        balls.push_back(ball);
    }
}

//--------------------------------------------------------------
void ofApp::update() {
	ofBackground(100, 100, 100);
	kinect.update();
    box2d.update();
    
    if (kinect.isFrameNew()) {
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/1, 50, false);
        
        //境界線の円をクリア
        for (int i = 0; i < contourCircles.size(); i++) {
            contourCircles[i]->destroy();
        }
        contourCircles.clear();
        //検出された物体(Blobs)の数だけ分析
        for (int i = 0; i < contourFinder.nBlobs; i++){
            for (int j = 0; j < contourFinder.blobs[i].pts.size(); j += 4) {
                //輪郭線にそって等間隔に座標を抽出
                ofPoint pos = contourFinder.blobs[i].pts[j];
                //輪郭線に並べるofxBox2dCircleを追加
                ofxBox2dCircle* circle = new ofxBox2dCircle();
                circle->setup(box2d.getWorld(), pos.x, pos.y, 4);
                //Vector配列contourCirclesに追加
                contourCircles.push_back(circle);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    int w = kinect.width;
    int h = kinect.height;
    
    ofSetColor(255, 255, 255);
    
    // draw particles
    drawBackgroundGraphics();
    
    // draw images from kinect
    if(bDrawPointCloud) {
        easyCam.begin();
        drawPointCloud();
        easyCam.end();
    } else {
        // draw from the live kinect
        contourFinder.draw(10, 320, 800, 400);
        ofNoFill();
        ofSetColor(255, 0, 0);
        ofPushMatrix();
//        ofScale(2, 2);
        for (int i = 0; i < contourCircles.size(); i++) {
            contourCircles[i]->draw();
        }
        ofPopMatrix();
    }
    
    // draw instructions
    ofSetColor(255, 255, 255);
    stringstream reportStream;
    
    if(kinect.hasAccelControl()) {
        reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
        << ofToString(kinect.getMksAccel().y, 2) << " / "
        << ofToString(kinect.getMksAccel().z, 2) << endl;
    } else {
        reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
		<< "motor / led / accel controls are not currently supported" << endl << endl;
    }
    
	reportStream << "l : gain scale, h : down scale | " << scale << endl
    << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
	<< "set near threshold " << nearDepthThreshold << " (press: + -)" << endl
	<< "set far threshold " << farDepthThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
	<< ", fps: " << ofGetFrameRate() << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl;

    if(kinect.hasCamTiltControl()) {
    	reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }
    
	ofDrawBitmapString(reportStream.str(), 20, 652);
    
}

void ofApp::drawPointCloud() {
    int w = kinect.width;
	int h = kinect.height;
    
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
    
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > nearDepthThreshold && kinect.getDistanceAt(x, y) < farDepthThreshold) {
            } else {
                ofColor red(255, 0, 0);
                mesh.addColor(red);
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
    
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(1, -1, -1);
	ofTranslate(translateX, translateY, -scale); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
}

void ofApp::drawBackgroundGraphics() {
    for (int i = 0, len = balls.size(); i < len; i++) {
        balls[i]->update();
        balls[i]-> draw();
    }
}

//--------------------------------------------------------------
void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
		case'p':
			bDrawPointCloud = !bDrawPointCloud;
			break;
            
		case '>':
		case '.':
			farDepthThreshold ++;
			break;
			
		case '<':
		case ',':
			farDepthThreshold --;
			break;
			
		case '+':
		case '=':
			nearDepthThreshold ++;
			break;
			
		case '-':
			nearDepthThreshold --;
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
            
        case 'h':
            scale -= 10;
            break;
            
        case 'l':
            scale += 10;
            break;
			
		case '1':
			kinect.setLed(ofxKinect::LED_GREEN);
			break;
			
		case '2':
			kinect.setLed(ofxKinect::LED_YELLOW);
			break;
			
		case '3':
			kinect.setLed(ofxKinect::LED_RED);
			break;
			
		case '4':
			kinect.setLed(ofxKinect::LED_BLINK_GREEN);
			break;
			
		case '5':
			kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
			break;
			
		case '0':
			kinect.setLed(ofxKinect::LED_OFF);
			break;
            
        case OF_KEY_UP:
            translateY -= 5;
            break;
            
        case OF_KEY_DOWN:
            translateY += 5;
            break;
            
        case OF_KEY_RIGHT:
            translateX += 5;
            break;
            
        case OF_KEY_LEFT:
            translateX -= 5;
            break;
			
//		case OF_KEY_UP:
//			angle++;
//			if(angle>30) angle=30;
//			kinect.setCameraTiltAngle(angle);
//			break;
//			
//		case OF_KEY_DOWN:
//			angle--;
//			if(angle<-30) angle=-30;
//			kinect.setCameraTiltAngle(angle);
//			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{}
