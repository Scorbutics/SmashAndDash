#pragma once
#include "ControlStatement.h"
namespace ska {
	class CommandIf : public ControlStatement {
	public:
		explicit CommandIf(EntityManager& e);
		void operator=(const CommandIf&) = delete;
		virtual ~CommandIf();

		virtual std::string analyzeLine(ScriptComponent& script, std::stringstream& streamCmd, std::vector<std::string>& args) override;
		virtual const std::string& getCmdName() override;
	};
}
