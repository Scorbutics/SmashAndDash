#pragma once
#include "AbstractFunctionCommand.h"
class CommandTranslationCamera :
	public AbstractFunctionCommand
{
public:
	CommandTranslationCamera();
	virtual ~CommandTranslationCamera();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

