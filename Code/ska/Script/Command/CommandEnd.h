#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandEnd : public AbstractFunctionCommand {
	public:
		explicit CommandEnd(EntityManager& e);
		void operator=(const CommandEnd&) = delete;
		virtual ~CommandEnd();

		static const std::string& getCmdName();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
