#pragma once

#include "ofxThread.h"
#include "GpsData.h"
#include "ofSerial.h"

#define SERIAL_READLINE_SLEEP 10

class GpsLog : public ofxThread {
protected:
	ofSerial gpsSerialData, gpsSerialControl;
	
	GpsData workingData, stableData;
	string nmeaMessage;

	float lastInput;
	
	void threadedFunction() {
		while(isThreadRunning()) {
			nmeaMessage = readLine();
			if(workingData.parseOutput(nmeaMessage)) {
				stableData = workingData;
			}
		}
	}
	string readLine() {
		string available = "";
		char curChar = '\0';
		while(isThreadRunning() && curChar != '\n') {
			if(gpsSerialData.available()) {
				lastInput = ofGetElapsedTimef();
				gpsSerialData.readBytes((unsigned char*) &curChar, 1);
				if(curChar == '\n') {
					break;
				} else {
					available += curChar;
				}
			} else {
				ofSleepMillis(SERIAL_READLINE_SLEEP);
			}
		}
		return available;
	}
public:	
	~GpsLog() {
		gpsSerialData.close();
		gpsSerialControl.close();
	}
	void setup() {
		if(!gpsSerialControl.setup(4, 9600)) {
			ofLog(OF_LOG_FATAL_ERROR, "Cannot connect to the GPS control.");
		} else {
			startStream();
			if(!gpsSerialData.setup(5, 9600)) {
				ofLog(OF_LOG_FATAL_ERROR, "Cannot connect to the GPS data stream.");
			} else {
				ofLog(OF_LOG_VERBOSE, "Connected to internal GPS stream.");
			}		}
		lastInput = ofGetElapsedTimef();
	}
	const GpsData& getData() {
		return stableData;
	}
	const string& getLatestMessage() {
		return nmeaMessage;
	}
	float idleTime() const {
		return ofGetElapsedTimef() - lastInput;
	}
	void startStream() {
		string cmdstr = "AT_OGPS=1"; // something like this
		unsigned char* cmduc = new unsigned char[cmdstr.size()];
		memcpy(cmduc, cmdstr.c_str(), cmdstr.size());
		gpsSerialControl.writeBytes(cmduc, cmdstr.size());
		delete [] cmduc;
	}
};
