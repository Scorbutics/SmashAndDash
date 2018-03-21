#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandShop :
	public ska::AbstractFunctionCommand
{
public:
	CommandShop(ska::EntityManager& entityManager);
	virtual ~CommandShop();

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

