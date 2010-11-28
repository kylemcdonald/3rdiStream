#include "testApp.h"

void testApp::setup(){
	delayTimer.setPeriod(delay);
	
#ifdef USE_NETBOOK
	camera.setDeviceID(1);
#endif
	
	camera.initGrabber(camWidth, camHeight);
	lastFrame.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
}

void testApp::update(){
	camera.grabFrame();
	if(camera.isFrameNew() && delayTimer.tick()) {
		int n = camWidth * camHeight * 3;
		memcpy(lastFrame.getPixels(), camera.getPixels(), n);
		lastFrame.update();
	}
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

