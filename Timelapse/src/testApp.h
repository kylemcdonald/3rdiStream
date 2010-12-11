#pragma once

#include "ofMain.h"
#include "DelayTimer.h"
#include "GpsLog.h"
#include "ofxXmlSettings.h"
#include "ofxFileHelper.h"
#include "Poco/DateTimeFormatter.h"

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();

	void startCapture();
	void stopCapture();
	void grabFrame();
	void saveLastFrame();
	string getDaystamp();
	string getTimestamp();
	void ensureDirectory(string path, bool relativeToData = true);

	ofVideoGrabber camera;
	ofImage lastFrame, lastFrameResized;

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
	
	GpsLog gps;
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
};
