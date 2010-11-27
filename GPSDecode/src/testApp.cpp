#include "testApp.h"

void testApp::setup(){
	ifstream file;
	file.open(ofToDataPath("gps.txt").c_str());
	if(file.is_open()) {
		while(!file.eof()) {
			file.ignore(1);
			string sentence;
			file >> sentence;
			gpsData.parseOutput(sentence);
		}
	}
}

void testApp::update(){
}

void testApp::draw(){
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

