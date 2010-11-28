#pragma once

#include "ofMain.h"
#include "DelayTimer.h"

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	ofVideoGrabber camera;
	ofImage lastFrame;
	
	static const int camWidth = 640;
	static const int camHeight = 480;
#ifdef USE_NETBOOK
	static const float delay = 60;
#else
	static const float delay = 1;
#endif
	DelayTimer delayTimer;
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
};
