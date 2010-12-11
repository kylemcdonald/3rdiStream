#pragma once

#include "ofMain.h"

class GpsData {
protected:
	vector<string> split(const string &source, char separator) {
		vector<string> elements;
        stringstream stream(source);
        string item;
        while(getline(stream, item, separator)) {
                elements.push_back(item);
        }
        return elements;
	}

	bool complete;
	int satellites;
	
	void readFixedWidth(stringstream& stream, int width, int& val) {
		char str[width + 1];
		stream.read(str, width);
		stringstream restream(str);
		restream >> val;
	}
	
	void parseTime(const string& str, int& hours, int& minutes, float& seconds) {
		stringstream stream(str);
		readFixedWidth(stream, 2, hours);
		readFixedWidth(stream, 2, minutes);
		stream >> seconds;
	}
	
	void parseLatitude(const string& str, int& degrees, float& minutes) {
		stringstream stream(str);
		readFixedWidth(stream, 2, degrees);
		stream >> minutes;
	}
	
	void parseLongitude(const string& str, int& degrees, float& minutes) {
		stringstream stream(str);
		readFixedWidth(stream, 3, degrees);
		stream >> minutes;
	}
		
	void parseDate(const string& date) {
		stringstream stream(date);
		readFixedWidth(stream, 2, dayOfMonth);
		readFixedWidth(stream, 2, month);
		readFixedWidth(stream, 2, year);
	}
	
public:
	int hours, minutes;
	float seconds;
	int latDegrees, lonDegrees;
	float latMinutes, lonMinutes;
	string latReference, lonReference;
	float altitude;
	float heading;
	float knots;
	int dayOfMonth, month, year;
	
	GpsData() :
	hours(0), minutes(0), seconds(0),
	latDegrees(0), lonDegrees(0),
	latMinutes(0), lonMinutes(0),
	altitude(0),
	knots(0),
	heading(0),
	dayOfMonth(0), month(0), year(0),
	satellites(0),
	complete(false) {
	}
	
	bool parseOutput(string sentence) {
		vector<string> tokens = split(sentence, ',');
		complete = false;
		if(tokens.size() > 10 && tokens[0] == "$GPGGA") {
			satellites = ofToInt(tokens[7]);
			if(satellites > 0) {
				parseTime(tokens[1], hours, minutes, seconds);
				parseLatitude(tokens[2], latDegrees, latMinutes);
				parseLongitude(tokens[4], lonDegrees, lonMinutes);
                latReference = tokens[3];
                lonReference = tokens[5];
				altitude = ofToFloat(tokens[9]);
			}
		}
		if(tokens.size() > 10 && tokens[0] == "$GPRMC") {
			knots = ofToFloat(tokens[7]);
			heading = ofToFloat(tokens[8]);
			parseDate(tokens[9]);
			complete = true;
		}
		return ready();
	}
	
	bool ready() {
		return complete && satellites > 0;
	}
};
