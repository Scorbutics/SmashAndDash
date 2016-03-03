#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandAddPokemon :
	public ska::AbstractFunctionCommand
{
public:
	CommandAddPokemon();
	virtual ~CommandAddPokemon();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

