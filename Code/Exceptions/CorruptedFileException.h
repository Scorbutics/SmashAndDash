#pragma once
#include "InputException.h"

class CorruptedFileException :
	public InputException
{
public:

	CorruptedFileException(std::string message) : InputException(message, "CorruptedFileException") {
	}

	virtual ~CorruptedFileException() {
	}
};