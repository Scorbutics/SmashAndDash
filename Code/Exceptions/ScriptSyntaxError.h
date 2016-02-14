#pragma once
#include "ScriptException.h"
class ScriptSyntaxError :
	public ScriptException
{
public:

	ScriptSyntaxError(std::string message) : ScriptException(message)
	{
	}

	virtual ~ScriptSyntaxError()
	{
	}
};

