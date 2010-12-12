#include "testApp.h"

void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofDisableArbTex();
    
    captureFramerate = 1. / 5;
    lastTime = ofGetElapsedTimef();
    
    ids.OpenCamera();
    img.setUseTexture(true);
}

testApp::~testApp() {
    ids.CloseCamera();
}

void testApp::update(){
    float curTime = ofGetElapsedTimef();
    if((int) (curTime * captureFramerate) != (int) (lastTime * captureFramerate)) {
        cout << "capturing image" << endl;
        ids.snapImage(img);
        
        img.update();
        img.saveImage(ofToString(ofGetElapsedTimef()) + ".jpg");
    }
    lastTime = curTime;
}
    
void testApp::draw() {
    ofDisableAlphaBlending();
    ofBackground(0, 0, 0);
    ofSetColor(255);
    img.draw(0, 0, ofGetWidth(), ofGetHeight());
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

