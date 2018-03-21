#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandHideGUI :
	public ska::AbstractFunctionCommand
{
public:
	CommandHideGUI(ska::EntityManager& entityManager);
	virtual ~CommandHideGUI();

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

