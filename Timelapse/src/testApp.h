#pragma once

#include "ofMain.h"
#include "DelayTimer.h"
#include "FtpUpdate.h"
#include "ofxXmlSettings.h"
#include "ofxFileHelper.h"

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();

	void enableCamera();
	void disableCamera();
	bool grabFrame();
	void saveLastFrame();
	string getDaystamp();
	string getTimestamp();
	void ensureDirectory(string path, bool relativeToData = true);

	ofVideoGrabber camera;
	ofImage lastFrame, lastFrameResized;

	int camWidth, camHeight;
	int resizedWidth, resizedHeight;

    float delay;
	DelayTimer delayTimer;

	// This is how long the app waits between successive attempts at getting a new frame.
	static const int cameraFrameWait = 50; // in milliseconds
	static const float maxWaitingTime = 5; // in seconds
	
	FtpUpdate ftpUpdate;
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
};
