#include "testApp.h"

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	gps.setup(true, "ISP.CINGULAR");

/*
	gpsControl.enumerateDevices();
	gpsControl.setup("COM3", 9600);
	send("AT_OGPS=0\r\n");
	send("AT_OGPSP = 7,2\r\n");
	send("AT_OGPSCONT = 1, \"IP\", \"ISP.CINGULAR\"\r\n");
	send("AT_OGPSLS = 1, \"http://supl.nokia.com\"\r\n");
	send("AT_OGPS=2\r\n");

	gpsControl.close();

	gpsDataSerial.setup("COM5", 9600);
	*/
}

void testApp::send(string msg) {
	char* curMsg = new char[msg.size()];
	memcpy(curMsg, msg.c_str(), msg.size());
	gpsControl.writeBytes((unsigned char*) curMsg, msg.size());
	delete curMsg;

	while(!gpsControl.xavailable()) {
		ofLog(OF_LOG_VERBOSE, "Waiting for a response from GPS Control.");
		ofSleepMillis(100);
	}

	while(gpsControl.xavailable()) {
		int size = gpsControl.xavailable();
		char* buffer = new char[size + 1];
		gpsControl.readBytes((unsigned char*) buffer, size);
		buffer[size] = '\0';
		ofLog(OF_LOG_VERBOSE, "Received a response from GPS Control.");
	}
}

void testApp::update() {
	/*
	while(gpsDataSerial.xavailable()) {
		int size = gpsDataSerial.xavailable();
		char* buffer = new char[size + 1];
		gpsDataSerial.readBytes((unsigned char*) buffer, size);
		buffer[size] = '\0';
		cout << "gps: '" << buffer << "'" << endl;
	}
	*/
}

testApp::~testApp() {
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

