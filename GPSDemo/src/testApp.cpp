#include "testApp.h"

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	//gps.setup(true, "ISP.CINGULAR");

	gpsControl.enumerateDevices();
	gpsControl.setup("COM3", 9600);
	char curMsg[] = "AT_OGPS=2\r\n";
	gpsControl.writeBytes((unsigned char*) curMsg, 11);
	int curAvailable = gpsControl.available();
	cout << "curAvailable: " << curAvailable << endl;
	while(!gpsControl.available()) {
		cout << "waiting for a response" << endl;
		ofSleepMillis(100);
	}
	while(gpsControl.available()) {
		int size = gpsControl.available();
		char* buffer = new char[size + 1];
		gpsControl.readBytes((unsigned char*) buffer, size);
		buffer[size] = '\0';
		cout << "just received '" << buffer << "'" << endl;
	}
}

void testApp::update() {
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

