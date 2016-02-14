#pragma once
#include "GenericException.h"
class ScriptException :
	public GenericException
{
public:

	ScriptException(std::string message) : GenericException(message) {
	}

	virtual ~ScriptException() {
	}
};

