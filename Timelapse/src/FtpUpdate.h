#pragma once

#include "curl.h"

class FtpUpdate {
protected:
	string address, username, password;
	string localDirectory, remoteDirectory;
public:
	void setup(string address, string username, string password) {
		this->address = address;
		this->username = username;
		this->password = password;
	}
	void update(string localDirectory, string remoteDirectory) {
		ofLog(OF_LOG_VERBOSE, "Updating " + address + "/" + remoteDirectory + " from " + localDirectory);
		curl_global_init(CURL_GLOBAL_ALL);
		curl_global_cleanup();
	}
};