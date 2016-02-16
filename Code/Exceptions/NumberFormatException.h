#pragma once
#include "GenericException.h"
class NumberFormatException :
	public GenericException
{
public:
	NumberFormatException(std::string message) : GenericException(message, "NumberFormatException") {
	}

	virtual ~NumberFormatException() {
	}
};

