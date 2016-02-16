#pragma once
#include "InputException.h"

class InvalidPathException :
	public InputException
{
public:

	InvalidPathException(std::string message) : InputException(message, "InvalidPathException") {
	}

	virtual ~InvalidPathException() {
	}
};

