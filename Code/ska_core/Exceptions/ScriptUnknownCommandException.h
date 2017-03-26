#pragma once
#include "ScriptException.h"

namespace ska {
	class ScriptUnknownCommandException : public ScriptException {
	public:

		ScriptUnknownCommandException(std::string message) : ScriptException(message, "ScriptUnknownCommandException") {
		}

		virtual ~ScriptUnknownCommandException() {
		}
	};
}
