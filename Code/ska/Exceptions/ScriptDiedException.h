#pragma once
#include "ScriptException.h"

namespace ska {
	class ScriptDiedException : public ScriptException {
	public:

		ScriptDiedException(std::string message) : ScriptException(message, "ScriptDiedException"), m_script(message) {
		}

		const std::string& getScript() {
			return m_script;
		}

		virtual ~ScriptDiedException() {
		}
	private:
		std::string m_script;
	};
}
