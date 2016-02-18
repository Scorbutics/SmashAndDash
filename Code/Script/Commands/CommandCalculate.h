#pragma once
#include "AbstractFunctionCommand.h"
class CommandCalculate :
	public AbstractFunctionCommand
{
public:
	CommandCalculate();
	virtual ~CommandCalculate();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

