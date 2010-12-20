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
	string controlPort, dataPort;

	GpsData workingData, stableData;
	string nmeaMessage;

	float lastInput;

	void threadedFunction() {
		// opening serial ports inside this thread instead
		startStream();
		while(isThreadRunning()) {
			nmeaMessage = readLine();
			if(workingData.parseOutput(nmeaMessage)) {
				stableData = workingData;
			}
		}
	}
	string readAvailable() {
		int size = gpsSerialData.xavailable();
		unsigned char* buffer = new unsigned char[size + 1];
		gpsSerialData.readBytes(buffer, size);
		buffer[size] = '\0';
		stringstream cur;
		cur << buffer;
		return cur.str();
	}
	string readLine() {
		string available = "";
		char curChar = '\0';
		while(isThreadRunning() && curChar != '\n') {
			if(gpsSerialData.xavailable()) {
				lastInput = ofGetElapsedTimef();
				curChar = (unsigned char) gpsSerialData.readByte();
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
	void sendControl(string msg) {
		msg += "\r\n";

		char* curMsg = new char[msg.size()];
		memcpy(curMsg, msg.c_str(), msg.size());
		gpsSerialControl.writeBytes((unsigned char*) curMsg, msg.size());
		delete curMsg;

		while(!gpsSerialControl.xavailable()) {
			ofLog(OF_LOG_VERBOSE, "Waiting for a response from GPS Control.");
			ofSleepMillis(100);
		}

		while(gpsSerialControl.xavailable()) {
			int size = gpsSerialControl.xavailable();
			char* buffer = new char[size + 1];
			gpsSerialControl.readBytes((unsigned char*) buffer, size);
			buffer[size] = '\0';
			ofLog(OF_LOG_VERBOSE, "Received a response from GPS Control.");
		}
	}
public:
	~GpsLogger() {
		stopThread();
		gpsSerialData.close();
		gpsSerialControl.close();
	}
	void setup(string controlPort, string dataPort, bool useAgps, string apn) {
		this->controlPort = controlPort;
		this->dataPort = dataPort;
		this->useAgps = useAgps;
		this->apn = apn;

		gpsSerialControl.enumerateDevices();
		lastInput = ofGetElapsedTimef();
		startThread(true);
	}
	GpsData getData() {
		GpsData curData = stableData;
		return curData;
	}
	string getLatestMessage() {
		return nmeaMessage;
	}
	float idleTime() const {
		return ofGetElapsedTimef() - lastInput;
	}
	void startStream() {
		if(!gpsSerialControl.setup(controlPort, 9600)) {
			ofLog(OF_LOG_FATAL_ERROR, "Cannot connect to the GPS control.");
			return;
		} else {
			ofLog(OF_LOG_VERBOSE, "Connected to the GPS control.");
		}

		sendControl("AT_OGPS=0");
		if(useAgps) {
			sendControl("AT_OGPSP = 7,2");
			sendControl("AT_OGPSCONT = 1, \"IP\", \"" + apn + "\"");
			sendControl("AT_OGPSLS = 1, \"http://supl.nokia.com\"");
		}
		sendControl("AT_OGPS=2");

		ofLog(OF_LOG_VERBOSE, "Done setting up GPS control.");

		gpsSerialControl.close();

		ofLog(OF_LOG_VERBOSE, "Closed GPS control.");

		if(!gpsSerialData.setup(dataPort, 9600)) {
			ofLog(OF_LOG_FATAL_ERROR, "Cannot connect to the GPS data stream.");
		} else {
			ofLog(OF_LOG_VERBOSE, "Connected to internal GPS stream.");
		}
	}
};
