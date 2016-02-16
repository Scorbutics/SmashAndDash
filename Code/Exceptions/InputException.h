#pragma once
#include "GenericException.h"
class InputException :
	public GenericException
{
public:

	InputException(std::string message) : GenericException(message, "InputException") {
	}

	InputException(std::string message, const std::string& type) : GenericException(message, type) {
	}

	virtual ~InputException() {
	}
};

