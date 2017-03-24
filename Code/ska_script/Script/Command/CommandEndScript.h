#pragma once
#include <vector>
#include "CommandEnd.h"

namespace ska {
	class CommandEndScript :
		public CommandEnd {
	public:
		explicit CommandEndScript(EntityManager& e);
		void operator=(const CommandEndScript&) = delete;
		virtual ~CommandEndScript();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};

}
