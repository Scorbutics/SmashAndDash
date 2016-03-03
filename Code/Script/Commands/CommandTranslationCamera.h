#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandTranslationCamera :
	public ska::AbstractFunctionCommand
{
public:
	CommandTranslationCamera();
	virtual ~CommandTranslationCamera();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

