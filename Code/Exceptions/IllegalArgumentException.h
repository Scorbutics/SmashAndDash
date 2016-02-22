#pragma once
#include "GenericException.h"

class IllegalArgumentException : public GenericException {
public:

	IllegalArgumentException(std::string message) : IllegalArgumentException(message, "IllegalArgumentException") {
	}

	IllegalArgumentException(std::string message, const std::string& type) : GenericException(message, type) {
	}

	virtual ~IllegalArgumentException() {
	}
};