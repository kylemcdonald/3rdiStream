#include "testApp.h"

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	gps.setup("COM3", "COM5", true, "ISP.CINGULAR");

	ofSetFrameRate(5);
}

void testApp::update() {
	string cur = gps.getLatestMessage();
	cout << cur << endl;
}

void testApp::draw() {
}

void testApp::keyPressed(int key) {
}

void testApp::keyReleased(int key) {
}

void testApp::mouseMoved(int x, int y ) {
}

void testApp::mouseDragged(int x, int y, int button) {
}

void testApp::mousePressed(int x, int y, int button) {
}

void testApp::mouseReleased(int x, int y, int button) {
}

void testApp::windowResized(int w, int h) {
}

