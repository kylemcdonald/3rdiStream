#pragma once

#include "ofMain.h"
#include "DelayTimer.h"
#include "ofxGps.h"
#include "ofxXmlSettings.h"
#include "ofxFileHelper.h"
#include "Poco/DateTimeFormatter.h"
#include "ofxImage.h"

#ifdef USE_NETBOOK
#include "ofxIds.h"
#endif

#define RESIZED_DIR "3rdiStream/resized/"
#define ORIGINAL_DIR "3rdiStream/original/"

class testApp : public ofBaseApp {
public:
	~testApp();
	void setup();
	void update();
	void draw();

	void startUpload();

	void startCapture();
	void stopCapture();
	void grabFrame();
	void saveLastFrame();

	string daystamp, timestamp;
	string getDaystamp();
	string getTimestamp();
	void ensureDirectory(string path, bool relativeToData = true);

	bool makeExivScript(string scriptFile);

	bool useIds;
#ifdef USE_NETBOOK
	ofxIds ids;
#endif

	ofVideoGrabber camera;
	ofxImage lastFrame;
	ofImage lastFrameResized;

	int deviceId;
	int camWidth, camHeight;
	int resizedWidth, resizedHeight;

	float photoInterval;
	DelayTimer photoTimer;

	float uploadInterval;
	DelayTimer uploadTimer;
	float startWaiting;
	bool capturing;

	// This is how long the app waits between successive attempts at getting a new frame.
	static const int cameraFrameWait = 100; // in milliseconds
	float photoTimeout;
	bool rotateImage;

	GpsLogger gps;
	float gpsTimeout;

	string username, password, address;

	ofSoundPlayer shutterSound;

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
};
