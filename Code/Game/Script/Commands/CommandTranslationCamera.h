#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

class CommandTranslationCamera :
	public ska::AbstractFunctionCommand {
public:
	CommandTranslationCamera(ska::EntityManager& entityManager);
	virtual ~CommandTranslationCamera() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

