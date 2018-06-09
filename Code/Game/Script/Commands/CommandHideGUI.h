#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandHideGUI :
	public ska::AbstractFunctionCommand
{
public:
	CommandHideGUI(ska::EntityManager& entityManager);
	virtual ~CommandHideGUI() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

