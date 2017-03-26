#pragma once
#include "ScriptException.h"

namespace ska {
	class ScriptNotActivatedException : public ScriptException {
	public:

		ScriptNotActivatedException(std::string message) : ScriptException(message, "ScriptNotActivatedException") {
		}

		virtual ~ScriptNotActivatedException() {
		}
	};
}
