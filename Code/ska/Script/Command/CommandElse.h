#pragma once
#include "ControlStatement.h"
namespace ska {
	class CommandElse :
		public ControlStatement
	{
	public:
		CommandElse(EntityManager& e);
		virtual ~CommandElse();

		virtual std::string analyzeLine(ScriptComponent& script, std::stringstream& streamCmd, std::vector<std::string>& args) override;
		virtual const std::string& getCmdName() override;
	};
}
