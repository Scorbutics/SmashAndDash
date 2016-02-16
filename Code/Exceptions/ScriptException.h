#pragma once
#include "GenericException.h"
class ScriptException :
	public GenericException
{
public:

	ScriptException(std::string message) : GenericException(message, "ScriptException") {
	}

	ScriptException(std::string message, const std::string& type) : GenericException(message, type) {
	}

	virtual ~ScriptException() {
	}
};

