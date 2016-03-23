#pragma once
#include "ControlStatement.h"

namespace ska {
	class CommandElseEnd : public ControlStatement {

	public:
		virtual const std::string& getCmdName() {
			return ControlStatement::getCommandEndIf();
		}

	protected:
		virtual std::string analyzeLine(ScriptComponent& script, std::stringstream& streamCmd, std::vector<std::string>& args) override
		{
			return "";
		}

		virtual int argumentsNumber() {
			return 0;
		}
	};
}