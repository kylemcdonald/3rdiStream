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
		cmd += "\r\n\0";
		unsigned char* cmduc = new unsigned char[cmd.size()];
		memcpy(cmduc, cmd.c_str(), cmd.size());
		gpsSerialControl.writeBytes(cmduc, cmd.size());
		delete [] cmduc;
		/*
		ofLog(OF_LOG_VERBOSE, "Flushing GPS Control output.");
		gpsSerialControl.flush(false, true);
		ofLog(OF_LOG_VERBOSE, "Done flushing.");
		*/
	}
	void listenControl(string msg) {
		string ret;
		ofLog(OF_LOG_VERBOSE, "Waiting for some kind of response from GPS Control.");
		/*while(!gpsSerialControl.available()) {
		}*/
		while(gpsSerialControl.available()) {
			ofLog(OF_LOG_VERBOSE, "Reading a byte from GPS Control.");
			char cur = gpsSerialControl.readByte();
			ret += cur;
			ofSleepMillis(100);
			ofLog(OF_LOG_VERBOSE, "Received '" + ret + "' from GPS Control.");
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

		if(useAgps) {
			sendControl("AT_OGPSP = 7,2");
			listenControl("OK\r\n");
			sendControl("AT_OGPSCONT = 1, \"IP\", \"" + apn + "\"");
			listenControl("OK\r\n");
			sendControl("AT_OGPSLS = 1, \"http://supl.nokia.com\"");
			listenControl("OK\r\n");
		}
		sendControl("AT_OGPS = 2");
		listenControl("OK\r\n");

		ofLog(OF_LOG_VERBOSE, "Done setting up GPS control.");

		gpsSerialControl.close();

		ofLog(OF_LOG_VERBOSE, "Closed GPS control.");

		gpsSerialData.close();
		if(!gpsSerialData.setup("COM4", 9600)) {
			ofLog(OF_LOG_FATAL_ERROR, "Cannot connect to the GPS data stream.");
		} else {
			ofLog(OF_LOG_VERBOSE, "Connected to internal GPS stream.");
		}
	}
};
