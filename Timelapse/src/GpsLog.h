#pragma once

#include "ofxThread.h"
#include "GpsData.h"
#include "ofSerial.h"

#define SERIAL_READLINE_SLEEP 10

class GpsLog : public ofxThread {
protected:
	ofSerial gpsSerialData, gpsSerialControl;
	
	bool useAgps;
	string apn;
	
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
			    while(gpsSerialControl.available()) {
                    gpsSerialControl.readByte();
			    }
				ofSleepMillis(SERIAL_READLINE_SLEEP);
			}
		}
		return available;
	}
	void sendControl(string cmd) {
	    cmd += "\r\n";
		unsigned char* cmduc = new unsigned char[cmd.size()];
		memcpy(cmduc, cmd.c_str(), cmd.size());
		gpsSerialControl.flush();
		gpsSerialControl.writeBytes(cmduc, cmd.size());
		delete [] cmduc;
	}
public:	
	~GpsLog() {
		gpsSerialData.close();
		gpsSerialControl.close();
	}
	void setup(bool useAgps, string apn) {
	    this->useAgps = useAgps;
	    this->apn = apn;
	    
	    gpsSerialControl.enumerateDevices();
		if(!gpsSerialControl.setup("COM5", 9600)) {
			ofLog(OF_LOG_FATAL_ERROR, "Cannot connect to the GPS control.");
		} else {
			ofLog(OF_LOG_VERBOSE, "Connected to the GPS control.");
			startStream();
		}
        if(!gpsSerialData.setup("COM4", 9600)) {
            ofLog(OF_LOG_FATAL_ERROR, "Cannot connect to the GPS data stream.");
        } else {
            ofLog(OF_LOG_VERBOSE, "Connected to internal GPS stream.");
        }
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
	    if(useAgps) {
            sendControl("AT_OGPSP = 7,2");
            sendControl("AT_OGPSCONT = 1, IP, " + apn);
            sendControl("AT_OGPSLS = 1, http://supl.nokia.com");
	    }
	    sendControl("AT_OGPS = 2");
	}
};
