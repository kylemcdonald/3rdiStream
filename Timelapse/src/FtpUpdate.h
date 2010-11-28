#pragma once

#include "curl/curl.h"

class FtpUpdate {
protected:
	string address, username, password;
	string localDirectory, remoteDirectory;
public:
	void setup(string address, string username, string password, string localDirectory, string remoteDirectory) {
		this->address = address;
		this->username = username;
		this->password = password;
		this->remoteDirectory = remoteDirectory;
		this->localDirectory = localDirectory;
	}
	void update() {
	}
};