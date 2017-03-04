#pragma once
#include "ControlStatement.h"
namespace ska {
	class CommandElse :
		public ControlStatement {
	public:
		explicit CommandElse(EntityManager& e);
		void operator=(const CommandElse&) = delete;
		virtual ~CommandElse();

		virtual std::string analyzeLine(ScriptComponent& script, std::stringstream& streamCmd, std::vector<std::string>& args) override;
		virtual const std::string& getCmdName() override;
	};
}
