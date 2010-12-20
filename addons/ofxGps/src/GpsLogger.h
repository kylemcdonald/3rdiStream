#pragma once

#include "ofxThread.h"
#include "GpsData.h"
#include "ofxSerial.h"

#define SERIAL_READLINE_SLEEP 50

class GpsLogger : public ofxThread {
protected:
	ofxSerial gpsSerialData, gpsSerialControl;

	bool useAgps;
	string apn;

	GpsData workingData, stableData;
	string nmeaMessage;

	float lastInput;

	void threadedFunction() {
		while(isThreadRunning()) {
			nmeaMessage = readLine();
			if(workingData.parseOutput(nmeaMessage)) {
				lock();
				stableData = workingData;
				unlock();
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

		char* curMsg = new char[msg.size()];
		memcpy(curMsg, msg.c_str(), msg.size());
		gpsControl.writeBytes((unsigned char*) curMsg, msg.size());
		delete curMsg;

		while(!gpsControl.xavailable()) {
			ofLog(OF_LOG_VERBOSE, "Waiting for a response from GPS Control.");
			ofSleepMillis(100);
		}

		while(gpsControl.xavailable()) {
			int size = gpsControl.xavailable();
			char* buffer = new char[size + 1];
			gpsControl.readBytes((unsigned char*) buffer, size);
			buffer[size] = '\0';
			ofLog(OF_LOG_VERBOSE, "Received a response from GPS Control.");
		}
	}
public:
	~GpsLogger() {
		gpsSerialData.close();
		gpsSerialControl.close();
	}
	void setup(bool useAgps, string apn) {
		this->useAgps = useAgps;
		this->apn = apn;

		gpsSerialControl.enumerateDevices();
		lastInput = ofGetElapsedTimef();
		startStream();
		startThread(true);
	}
	GpsData getData() {
		lock();
		GpsData curData = stableData;
		unlock();
		return curData;
	}
	string getLatestMessage() {
		return nmeaMessage;
	}
	float idleTime() const {
		return ofGetElapsedTimef() - lastInput;
	}
	void startStream() {
		if(!gpsSerialControl.setup("COM3", 9600)) {
			ofLog(OF_LOG_FATAL_ERROR, "Cannot connect to the GPS control.");
			return;
		} else {
			ofLog(OF_LOG_VERBOSE, "Connected to the GPS control.");
		}

		sendControl("AT_OGPS=0");
		if(useAgps) {
			sendControl("AT_OGPSP = 7,2\r\n");
			sendControl("AT_OGPSCONT = 1, \"IP\", \"ISP.CINGULAR\"\r\n");
			sendControl("AT_OGPSLS = 1, \"http://supl.nokia.com\"\r\n");
		}
		sendControl("AT_OGPS=2");

		ofLog(OF_LOG_VERBOSE, "Done setting up GPS control.");

		gpsSerialControl.close();

		ofLog(OF_LOG_VERBOSE, "Closed GPS control.");

		if(!gpsSerialData.setup("COM4", 9600)) {
			ofLog(OF_LOG_FATAL_ERROR, "Cannot connect to the GPS data stream.");
		} else {
			ofLog(OF_LOG_VERBOSE, "Connected to internal GPS stream.");
		}
	}
};
