#pragma once
#include "AbstractCommand.h"
class AbstractFunctionCommand :
	public AbstractCommand
{
public:
	AbstractFunctionCommand();
	virtual ~AbstractFunctionCommand();

protected:
	virtual int argumentsNumber() = 0;
	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) = 0;

	std::string process(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::ifstream& fscript) override;
	virtual char getSeparator() override;
};

