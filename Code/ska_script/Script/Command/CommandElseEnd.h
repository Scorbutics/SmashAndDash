#pragma once
#include "ControlStatement.h"

namespace ska {
	class CommandElseEnd : public ControlStatement {

	public:
		explicit CommandElseEnd(EntityManager& e) : ControlStatement(e) { }
		void operator=(const CommandElseEnd&) = delete;
		virtual const std::string& getCmdName() override{
			return getCommandEndIf();
		}

	protected:
		virtual std::string analyzeLine(ScriptComponent&, std::stringstream&, std::vector<std::string>&) override {
			return "";
		}

		virtual int argumentsNumber() {
			return 0;
		}
	};
}
