#pragma once

#include "ofMain.h"

/*
 ofxSerial differs from the usual ofSerial in that
 it always tries to close and re-setup the connection
 if for some reason it isn't inited.
 */

class ofxSerial : public ofSerial {
protected:
	string portName;
	int baudRate;

	void checkInited() {
		if(!ofSerial::bInited) {
			ofLog(OF_LOG_VERBOSE, "ofxSerial not inited, reconnecting to " + portName + ":" + ofToString(baudRate));
			ofSerial::close();
			ofSerial::setup(portName, baudRate);
		}
	}
public:
	bool setup(string portName, int baudRate) {
		this->portName = portName;
		this->baudRate = baudRate;
		ofLog(OF_LOG_VERBOSE, "Attempting to open " + portName);
		return ofSerial::setup(portName, baudRate);
	}
	int readBytes(unsigned char* buffer, int length) {
		checkInited();
		return ofSerial::readBytes(buffer, length);
	}
	int writeBytes(unsigned char* buffer, int length) {
		checkInited();
		return ofSerial::writeBytes(buffer, length);
	}
	int available() {
		checkInited();
		return available();
	}
	void flush(bool flushIn = true, bool flushOut = true) {
		checkInited();
		flush(flushIn, flushOut);
	}
	void enumerateDevices() {
#ifdef TARGET_WIN32
		enumerateWin32Ports();
		printf("ofSerial: listing devices (%i total)\n", nPorts);
		for (int i = 0; i < nPorts; i++){
			cout << "device " << i << " -- " << portNamesFriendly[i] << endl;
		}
#else
		ofSerial::enumerateDevices();
#endif
	}
	int xavailable() {
#ifdef TARGET_WIN32
		COMSTAT stat;
		DWORD err;
		if(hComm!=INVALID_HANDLE_VALUE){
			//ofLog(OF_LOG_VERBOSE, "hComm is a valid handle value");
			if(!ClearCommError(hComm, &err, &stat)){
				//ofLog(OF_LOG_VERBOSE, "!ClearCommError");
				return 0;
			} else {
				//ofLog(OF_LOG_VERBOSE, "ClearCommError");
				return stat.cbInQue;
			}
		} else {
			//ofLog(OF_LOG_VERBOSE, "hComm == INVALID_HANDLE_VALUE: " + ofToString((int) hComm));
			return 0;
		}
#else
		return ofSerial::available();
#endif
	}
};
