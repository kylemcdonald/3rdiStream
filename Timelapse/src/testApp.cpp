#include "testApp.h"

testApp::~testApp() {
#ifdef USE_NETBOOK
		ids.CloseCamera();
#endif
}

void testApp::setup() {
	ofDisableArbTex();

	shutterSound.loadSound("shutterSound.wav");

	ofSetLogLevel(OF_LOG_VERBOSE);

	ofxXmlSettings cameraSettings;
	cameraSettings.loadFile("cameraSettings.xml");
#ifdef USE_NETBOOK
	cameraSettings.pushTag("netbook");
#else
	cameraSettings.pushTag("debug");
#endif
	deviceId = cameraSettings.getValue("deviceId", 0);
	float resize = cameraSettings.getValue("resize", 0.);
	photoInterval = cameraSettings.getValue("photoInterval", 0.);
	photoTimeout = cameraSettings.getValue("photoTimeout", 0.);
	uploadInterval = cameraSettings.getValue("uploadInterval", 0.);
	useIds = cameraSettings.getValue("useIds", 0);
	gpsTimeout = cameraSettings.getValue("gpsTimeout", 1.);
	rotateImage = cameraSettings.getValue("rotateImage", 0);

	ofLog(OF_LOG_VERBOSE, "rotating images: " + ofToString(rotateImage));

	string apn;
	bool useAgps = cameraSettings.tagExists("apn");
	if(useAgps) {
		apn = cameraSettings.getValue("apn", "");
	}

	string controlPort = cameraSettings.getValue("controlPort", "");
	string dataPort = cameraSettings.getValue("dataPort", "");

	cameraSettings.popTag();

	ofSetFrameRate(1000. / cameraFrameWait);

	photoTimer.setPeriod(photoInterval);
	uploadTimer.setPeriod(uploadInterval);

	if(useIds) {
		camWidth = 3264;
		camHeight = 2448;
	} else {
		camera.listDevices();
		camWidth = 640;
		camHeight = 480;
	}
	resizedWidth = camWidth / resize;
	resizedHeight = camHeight / resize;

	ofLog(OF_LOG_VERBOSE, "original at " + ofToString(camWidth) + "x" + ofToString(camHeight));
	ofLog(OF_LOG_VERBOSE, "resized to " + ofToString(resizedWidth) + "x" + ofToString(resizedHeight));

	lastFrame.setUseTexture(false);

	startCapture();

	ofxXmlSettings serverSettings;
	serverSettings.loadFile("serverSettings.xml");

	address = serverSettings.getValue("address", "");
	username = serverSettings.getValue("username", "");
	password = serverSettings.getValue("password", "");
	ofLog(OF_LOG_VERBOSE, "Using FTP server " + username + "@" + address + " password: " + password);
	//ftpUpdate.setup(address, username, password);

	/*
	ofxXmlSettings transferSettings;
	transferSettings.loadFile("transferSettings.xml");
	string localDirectory = transferSettings.getValue("localDirectory", "");
	string remoteDirectory = transferSettings.getValue("remoteDirectory", "");
	ftpUpdate.update(localDirectory, remoteDirectory);
	*/

#ifdef USE_NETBOOK
	gps.setup(controlPort, dataPort, useAgps, apn);
	ofLog(OF_LOG_VERBOSE, "finished setting up serial (gps)");
#endif
}

void testApp::update() {
	if(photoTimer.tick()) {
		ofLog(OF_LOG_VERBOSE, "photoTimer ticked.");
		startCapture();
	}

	grabFrame();

	if(uploadTimer.tick()) {
		ofLog(OF_LOG_VERBOSE, "uploadTimer ticked.");
		startUpload();
	}
}

void testApp::startUpload() {
	// system("cd data & ftpsync-resized.bat & cd ..");
	if(daystamp != "") {
		string basePath = RESIZED_DIR + daystamp + "/" + timestamp + ".jpg";
		string localPath = ofToDataPath(basePath);
		// curl --ftp-create-dirs -T data/3rdiStream/timestamp.jpg -u user:pass server/3rdiStream/resized/daystamp
		string curlCommand = "curl --ftp-create-dirs -T " + localPath + " -u " + username + ":" + password + " ftp://" + address + "/" + basePath;
		if(system(curlCommand.c_str()) != 0) {
			ofLog(OF_LOG_VERBOSE, "failed to upload: " + curlCommand);
			// make a note here that things weren't uploaded. maybe a hidden file.
		} else {
			ofLog(OF_LOG_VERBOSE, "successful upload: " + curlCommand);
		}
	}
}

void testApp::startCapture() {
	if(!capturing) {
	    ofLog(OF_LOG_VERBOSE, "starting capture");
		bool success = false;
		if(useIds) {
#ifdef USE_NETBOOK
			success = ids.OpenCamera();
#endif
		} else {
			camera.setDeviceID(deviceId);
			success = camera.initGrabber(camWidth, camHeight);
		}
		if(success) {
			startWaiting = ofGetElapsedTimef();
			capturing = true;
		}
	}
}

void testApp::grabFrame() {
	if(capturing) {
		float waitingTime = ofGetElapsedTimef() - startWaiting;
		ofLog(OF_LOG_VERBOSE, "Grabbing frame from camera: " + ofToString(waitingTime));

		if(useIds) {
#ifdef USE_NETBOOK
			// grab a photo from the camera
			bool valid = ids.snapImage(lastFrame);
			for(int i = 0; !valid && i < maxCameraTries; i++) {
			    ofLog(OF_LOG_VERBOSE, "invalid (black) frame, trying again");
			    //ids.CloseCamera();
			    //ofSleepMillis(500);
			    //ids.OpenCamera();
			    ofSleepMillis(500);
					valid = ids.snapImage(lastFrame);
			}
			shutterSound.play();
#endif
			waitingTime = ofGetElapsedTimef() - startWaiting;
			ofLog(OF_LOG_VERBOSE, "Grabbed from via IDS API." + ofToString(waitingTime));
			saveLastFrame();
			stopCapture();
		} else {
			camera.grabFrame();

			if(camera.isFrameNew()) {
				shutterSound.play();
				ofLog(OF_LOG_VERBOSE, "Copying frame to lastFrame.");
				lastFrame.setFromPixels(camera.getPixels(), camWidth, camHeight, OF_IMAGE_COLOR);
				saveLastFrame();
				stopCapture();
			}

			if(waitingTime > photoTimeout) {
				ofLog(OF_LOG_VERBOSE, "Had to quit, camera is not responding.");
				stopCapture();
			}
		}
	}
}

void testApp::stopCapture() {
	if(useIds) {
#ifdef USE_NETBOOK
		ids.CloseCamera();
#endif
	} else {
		camera.close();
	}
	capturing = false;
}

string testApp::getDaystamp() {
	Poco::Timestamp curTime;
	return Poco::DateTimeFormatter::format(curTime, "%m-%d-%Y");
}

string testApp::getTimestamp() {
	Poco::Timestamp curTime;
	return Poco::DateTimeFormatter::format(curTime, "%H-%M-%S");
}

bool testApp::makeExivScript(string scriptFile) {
	GpsData gpsData = gps.getData();
	if(!gpsData.ready()) {
		return false;
	}
	fstream out;
	out.open(scriptFile.c_str(), ios_base::out | ios_base::trunc);
	out << "set Exif.GPSInfo.GPSVersionID Byte 0 0 2 2" << endl;
	out << "set Exif.GPSInfo.GPSLatitudeRef Ascii " << gpsData.latReference << endl;
	out << "set Exif.GPSInfo.GPSLatitude Rational " << gpsData.latDegrees << "/1 " <<
	(int) gpsData.latMinutes << "/1 " <<
	(int) (fmodf(gpsData.latMinutes, 1) * 60) << "/1" << endl;
	out << "set Exif.GPSInfo.GPSLongitudeRef Ascii " << gpsData.lonReference << endl;
	out << "set Exif.GPSInfo.GPSLongitude Rational " << gpsData.lonDegrees << "/1 " <<
	(int) gpsData.lonMinutes << "/1 " <<
	(int) (fmodf(gpsData.lonMinutes, 1) * 60) << "/1" << endl;
	out << "set Exif.GPSInfo.GPSAltitudeRef Byte " << (gpsData.altitude > 0 ? "0" : "1") << endl;
	out << "set Exif.GPSInfo.GPSAltitude Rational " << (int) (fabsf(gpsData.altitude)) << "/1" << endl;
	out.close();
	return true;
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
	daystamp = getDaystamp();
	timestamp = getTimestamp();

	if(rotateImage) {
		ofLog(OF_LOG_VERBOSE, "rotating image");
		lastFrame.rotate(180);
	} else {
		ofLog(OF_LOG_VERBOSE, "not rotating image");
	}

	ofLog(OF_LOG_VERBOSE, "saving images to " + daystamp + "/" + timestamp);

	string originalBase = ORIGINAL_DIR + daystamp;
	ensureDirectory(originalBase);
	string originalLocation = originalBase + "/" + timestamp + ".jpg";
	lastFrame.saveImage(originalLocation);

	lastFrameResized.setFromPixels(lastFrame.getPixels(), lastFrame.getWidth(), lastFrame.getHeight(), OF_IMAGE_COLOR, false);
	lastFrameResized.resize(resizedWidth, resizedHeight);

	string resizedBase = RESIZED_DIR + daystamp;
	ensureDirectory(resizedBase);
	string resizedLocation = resizedBase + "/" + timestamp + ".jpg";
	lastFrameResized.saveImage(resizedLocation);

	string scriptFile = "exiv2-gps.txt";
	if(makeExivScript(scriptFile)) {
		ofLog(OF_LOG_VERBOSE, "embedding GPS data");
		string originalCommand = "exiv2 -m " + scriptFile + " " + ofToDataPath(originalLocation);
		string resizedCommand = "exiv2 -m " + scriptFile + " " + ofToDataPath(resizedLocation);
		system(originalCommand.c_str());
		system(resizedCommand.c_str());
	}

	lastFrameResized.update();

	ofLog(OF_LOG_VERBOSE, "done saving and tagging");
}

void testApp::draw() {
	ofBackground(0, 0, 0);
	ofSetColor(255);
	lastFrameResized.draw(0, 0, ofGetWidth(), ofGetHeight());

#ifdef USE_NETBOOK
	ofSetColor(0);
	ofRect(5, 5, 400, 150);

	const GpsData& gpsData = gps.getData();
	stringstream gpsTime, gpsPosition;
	gpsTime << gpsData.hours << ":" << gpsData.minutes << ":" << gpsData.seconds;
	gpsPosition << gpsData.latDegrees << "* " << gpsData.latMinutes << "' " << gpsData.latReference << ", " <<
	gpsData.lonDegrees << "° " << gpsData.lonMinutes << "' " << gpsData.lonReference << " " <<
	(int) gpsData.altitude << "m";
	string status;
	if(gpsData.ready()) {
		status = "has fix";
	} else {
		status = "no fix";
	}
	ofSetColor(255, 0, 0);
	ofDrawBitmapString("GPS", 10, 20);
	ofSetColor(255);
	ofDrawBitmapString("status: " + status, 10, 40);
	ofDrawBitmapString("latest message: " + gps.getLatestMessage(), 10, 60);
	ofDrawBitmapString("time: " + gpsTime.str(), 10, 80);
	ofDrawBitmapString("satellites: " + ofToString(gpsData.satellites), 10, 100);
	ofDrawBitmapString("position: " + gpsPosition.str(), 10, 120);
	if(gps.idleTime() > gpsTimeout) {
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("GPS control is dead (" + ofToString((int) gps.idleTime()) + "s)", 10, 140);
		//gps.startStream();
	}
#endif
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

