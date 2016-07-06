#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandBlockCollision :
	public ska::AbstractFunctionCommand
{
public:
	CommandBlockCollision(ska::EntityManager& entityManager);
	
	virtual ~CommandBlockCollision();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};
