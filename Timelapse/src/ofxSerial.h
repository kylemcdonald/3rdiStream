#pragma once

#include "ofMain.h"

class ofxSerial : public ofSerial {
public:
    bool ready() {
        return bInited;
    }
};
