#include "testApp.h"

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	delayTimer.setPeriod(delay);
	
#ifdef USE_NETBOOK
	camera.setDeviceID(1);
#endif
	
	camera.initGrabber(camWidth, camHeight);
	lastFrame.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
}

void testApp::update(){
	if(delayTimer.tick()) {
		ofLog(OF_LOG_VERBOSE, "Timer ticked.");
		enableCamera();
		saveFrame();
		disableCamera();
	}
}

void testApp::enableCamera() {
}

bool testApp::saveFrame() {
	float startWaiting = ofGetElapsedTimef();
	while(true) {
		ofLog(OF_LOG_VERBOSE, "Grabbing frame from camera.");
		camera.grabFrame();
		float waitingTime = ofGetElapsedTimef() - startWaiting;
		
		if(camera.isFrameNew()) {
			ofLog(OF_LOG_VERBOSE, "Copying frame to lastFrame: " + ofToString(waitingTime));
			int n = camWidth * camHeight * 3;
			memcpy(lastFrame.getPixels(), camera.getPixels(), n);
			lastFrame.update();
			return true;
		}
		
		if(waitingTime > maxWaitingTime) {
			ofLog(OF_LOG_VERBOSE, "Had to quit, took too long to wait: " + ofToString(waitingTime));
			return false;
		}
		
		ofSleepMillis(cameraFrameWait);
	}
}

void testApp::disableCamera() {
}

void testApp::draw(){
	lastFrame.draw(0, 0);
}

void testApp::keyPressed(int key){
}

void testApp::keyReleased(int key){
}

void testApp::mouseMoved(int x, int y ){
}

void testApp::mouseDragged(int x, int y, int button){
}

void testApp::mousePressed(int x, int y, int button){
}

void testApp::mouseReleased(int x, int y, int button){
}

void testApp::windowResized(int w, int h){
}

