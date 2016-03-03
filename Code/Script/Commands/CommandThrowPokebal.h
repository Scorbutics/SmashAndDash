#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandThrowPokebal :
	public ska::AbstractFunctionCommand
{
public:
	CommandThrowPokebal();
	virtual ~CommandThrowPokebal();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

