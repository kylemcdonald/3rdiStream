#pragma once

#include "ofMain.h"

#include "ofxGps.h"

class testApp : public ofBaseApp {
public:
	void setup();
	~testApp();
	void update();
	void draw();

	void send(string msg);

	GpsLogger gps;
	ofxSerial gpsControl, gpsDataSerial;

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
};
