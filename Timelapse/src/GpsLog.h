#pragma once

#include "ofxThread.h"
#include "GpsData.h"
#include "ofSerial.h"

#define SERIAL_READLINE_SLEEP 10

class GpsLog : public ofxThread {
protected:
    ofSerial gpsSerialData;
	GpsData gpsData;
	
    void threadedFunction() {
        while(isThreadRunning()) {
            string nmeaMessage = readLine();
            cout << "incoming: " << nmeaMessage << endl;
            if(gpsData.parseOutput(nmeaMessage)) {
				cout << gpsData.hours << ":" << gpsData.minutes << ":" << gpsData.seconds << endl;
				cout << "latitude: " << gpsData.latDegrees << " degrees " << gpsData.latMinutes << "' " << gpsData.latReference << endl;
				cout << "longitude: " << gpsData.lonDegrees << " degrees " << gpsData.lonMinutes << "' " << gpsData.lonReference << endl;
				cout << "altitude: " << gpsData.altitude << " m" << endl;
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
    void setup() {
#ifdef USE_NETBOOK
        gpsSerialData.setup(5, 9600); // mayeb not 5?
#endif
    }
};
