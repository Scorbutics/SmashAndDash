#pragma once
#include "ControlStatement.h"

namespace ska {
	class CommandElseEnd : public ControlStatement {

	public:
		virtual const std::string& getCmdName() {
			return ControlStatement::getCommandEndIf();
		}

	protected:
		virtual std::string analyzeLine(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList) override
		{
			return "";
		}

		virtual int argumentsNumber() {
			return 0;
		}
	};
}