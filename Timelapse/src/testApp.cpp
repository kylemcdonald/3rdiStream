#include "testApp.h"

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	camWidth = 640;
	camHeight = 480;
	resizedWidth = 320;
	resizedHeight = 240;
	
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
		grabFrame();
		disableCamera();
	}
}

void testApp::enableCamera() {
}

bool testApp::grabFrame() {
	float startWaiting = ofGetElapsedTimef();
	while(true) {
		ofLog(OF_LOG_VERBOSE, "Grabbing frame from camera.");
		camera.grabFrame();
		float waitingTime = ofGetElapsedTimef() - startWaiting;
		
		if(camera.isFrameNew()) {
			ofLog(OF_LOG_VERBOSE, "Copying frame to lastFrame: " + ofToString(waitingTime));
			lastFrame.setFromPixels(camera.getPixels(), camWidth, camHeight, OF_IMAGE_COLOR);
			saveLastFrame();
			return true;
		}
		
		if(waitingTime > maxWaitingTime) {
			ofLog(OF_LOG_VERBOSE, "Had to quit, took too long to wait: " + ofToString(waitingTime));
			return false;
		}
		
		ofSleepMillis(cameraFrameWait);
	}
}

string testApp::getTimestamp() {
	stringstream timestamp;
	timestamp << ofGetMonth() << "-";
	timestamp << ofGetDay() << "-";
	timestamp << ofGetYear() << "-";
	timestamp << ofGetHours() << "-";
	timestamp << ofGetMinutes() << "-";
	timestamp << ofGetSeconds();
	return timestamp.str();
}

void testApp::saveLastFrame() {
	string timestamp = getTimestamp();

	lastFrame.saveImage("original/" + timestamp + ".jpg");
	
	lastFrameResized.clone(lastFrame);
	lastFrameResized.resize(resizedWidth, resizedHeight);
	lastFrameResized.update();
	lastFrameResized.saveImage("resized/" + timestamp + ".jpg");
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

