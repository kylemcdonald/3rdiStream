#include "testApp.h"

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofxXmlSettings cameraSettings;
	cameraSettings.loadFile("cameraSettings.xml");
#ifdef USE_NETBOOK
	cameraSettings.pushTag("netbook");
#else
	cameraSettings.pushTag("debug");
#endif
	int deviceId = cameraSettings.getValue("deviceId", 0);
	camWidth = cameraSettings.getValue("camWidth", 0);
	camHeight = cameraSettings.getValue("camHeight", 0);
	photoInterval = cameraSettings.getValue("photoInterval", 0.);
	uploadInterval = cameraSettings.getValue("uploadInterval", 0.);
	cameraSettings.popTag();
	
	resizedWidth = camWidth / 2;
	resizedHeight = camHeight / 2;
	
	photoTimer.setPeriod(photoInterval);
	uploadTimer.setPeriod(uploadInterval);
	
	camera.setDeviceID(deviceId);
	lastFrame.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
	
#ifdef USE_NETBOOK
    gpsSerialData.setup(5, 9600); // mayeb not 5
#endif

	/*
	 ofxXmlSettings serverSettings;
	 serverSettings.loadFile("serverSettings.xml");
	 string address = serverSettings.getValue("address", "");
	 string username = serverSettings.getValue("username", "");
	 string password = serverSettings.getValue("password", "");
	 ofLog(OF_LOG_VERBOSE, "Using FTP server " + username + "@" + address + " password: " + password);
	 ftpUpdate.setup(address, username, password);
	 
	 ofxXmlSettings transferSettings;
	 transferSettings.loadFile("transferSettings.xml");
	 string localDirectory = transferSettings.getValue("localDirectory", "");
	 string remoteDirectory = transferSettings.getValue("remoteDirectory", "");
	 ftpUpdate.update(localDirectory, remoteDirectory);
	 */
}

void testApp::update(){
	if(photoTimer.tick()) {
		ofLog(OF_LOG_VERBOSE, "Timer ticked.");
		enableCamera();
		grabFrame();
		disableCamera();
	}
	if(uploadTimer.tick()) {
#ifdef USE_NETBOOK
		system("cd data & ftpsync-resized.bat & cd ..");
#endif
	}
}

void testApp::enableCamera() {
	camera.initGrabber(camWidth, camHeight);
}

bool testApp::grabFrame() {
	float startWaiting = ofGetElapsedTimef();
	while(true) {
		float waitingTime = ofGetElapsedTimef() - startWaiting;
		ofLog(OF_LOG_VERBOSE, "Grabbing frame from camera: " + ofToString(waitingTime));
		camera.grabFrame();
		
		if(camera.isFrameNew()) {
			ofLog(OF_LOG_VERBOSE, "Copying frame to lastFrame.");
			lastFrame.setFromPixels(camera.getPixels(), camWidth, camHeight, OF_IMAGE_COLOR);
			saveLastFrame();
			return true;
		}
		
		if(waitingTime > maxWaitingTime) {
			ofLog(OF_LOG_VERBOSE, "Had to quit, took too long to wait.");
			return false;
		}
		
		ofSleepMillis(cameraFrameWait);
	}
}

void testApp::disableCamera() {
	camera.close();
}

string testApp::getDaystamp() {
	stringstream daystamp;
	daystamp << ofGetMonth() << "-";
	daystamp << ofGetDay() << "-";
	daystamp << ofGetYear();
	return daystamp.str();
}

string testApp::getTimestamp() {
	stringstream timestamp;
	timestamp << ofGetHours() << "-";
	timestamp << ofGetMinutes() << "-";
	timestamp << ofGetSeconds();
	return timestamp.str();
}

void testApp::ensureDirectory(string path, bool relativeToData) {
	if(relativeToData) {
		path = ofToDataPath(path);
	}
	File file(path);
	// createDirectories will not overwrite, but it will create
	// all necessary parent directories
	file.createDirectories();
}

void testApp::saveLastFrame() {
	string daystamp = getDaystamp();
	string timestamp = getTimestamp();
	
	string originalBase = "3rdiStream/original/" + daystamp;
	ensureDirectory(originalBase);
	lastFrame.saveImage(originalBase + "/" + timestamp + ".jpg");
	lastFrame.update();
	
	lastFrameResized.clone(lastFrame);
	lastFrameResized.resize(resizedWidth, resizedHeight);
	
	string resizedBase = "3rdiStream/resized/" + daystamp;
	ensureDirectory(resizedBase);
	lastFrameResized.saveImage(resizedBase + "/" + timestamp + ".jpg");
	lastFrameResized.update();
}

void testApp::draw(){
	ofBackground(0, 0, 0);
	ofSetColor(255);
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

