#pragma once
#include "ScriptException.h"

namespace ska {
	class ScriptDiedException : public ScriptException {
	public:

		ScriptDiedException(std::string message) : ScriptException(message, "ScriptDiedException") {
		}

		virtual ~ScriptDiedException() {
		}
	};
}