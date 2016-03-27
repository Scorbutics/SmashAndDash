#pragma once
#include "ControlStatement.h"
namespace ska {
	class CommandIf : public ControlStatement
	{
	public:
		CommandIf(EntityManager& e);
		virtual ~CommandIf();

		virtual std::string analyzeLine(ScriptComponent& script, std::stringstream& streamCmd, std::vector<std::string>& args) override;
		virtual const std::string& getCmdName() override;
	};
}
