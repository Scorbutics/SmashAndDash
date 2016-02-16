#pragma once
#include "ScriptException.h"

class ScriptNotActivatedException :
	public ScriptException
{
public:

	ScriptNotActivatedException(std::string message) : ScriptException(message, "ScriptNotActivatedException") {
	}

	virtual ~ScriptNotActivatedException() {
	}
};

