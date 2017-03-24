#pragma once
#include "ScriptException.h"

namespace ska {
	class ScriptSyntaxError : public ScriptException {
	public:

		ScriptSyntaxError(std::string message) : ScriptException(message, "ScriptSyntaxError")
		{
		}

		virtual ~ScriptSyntaxError() throw() {
		}
	};

}
