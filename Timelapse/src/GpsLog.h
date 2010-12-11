#pragma once

#include "ofxThread.h"
#include "GpsData.h"
#include "ofSerial.h"

#define SERIAL_READLINE_SLEEP 10

class GpsLog : public ofxThread {
protected:
    ofSerial gpsSerialData;
	
	GpsData workingData, stableData;
	string nmeaMessage;
	
    void threadedFunction() {
        while(isThreadRunning()) {
            nmeaMessage = readLine();
            if(workingData.parseOutput(nmeaMessage)) {
                stableData = workingData;
                /*
				cout << gpsData.hours << ":" << gpsData.minutes << ":" << gpsData.seconds << endl;
				cout << "latitude: " << gpsData.latDegrees << " degrees " << gpsData.latMinutes << "' " << gpsData.latReference << endl;
				cout << "longitude: " << gpsData.lonDegrees << " degrees " << gpsData.lonMinutes << "' " << gpsData.lonReference << endl;
				cout << "altitude: " << gpsData.altitude << " m" << endl;
				*/
            }
        }
    }
    string readLine() {
        string available = "";
        char curChar = '\0';
        while(isThreadRunning() && curChar != '\n') {
            if(gpsSerialData.available()) {
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
    }
    void setup() {
        if(!gpsSerialData.setup(5, 9600)) {
            ofLog(OF_LOG_FATAL_ERROR, "Cannot connect to the GPS stream. Try closing and re-opening GPS control.");
        } else {
            ofLog(OF_LOG_VERBOSE, "Connected to internal GPS stream.");
        }
    }
    const GpsData& getData() {
        return stableData;
    }
    const string& getLatestMessage() {
        return nmeaMessage;
    }
};
