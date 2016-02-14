#pragma once
#include "GenericException.h"
class InputException :
	public GenericException
{
public:

	InputException(std::string message) : GenericException(message) {
	}

	virtual ~InputException() {
	}
};

