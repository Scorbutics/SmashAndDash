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
	virtual bool execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) = 0;

	virtual bool process(const std::string& extendedName, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override;
	virtual char getSeparator() override;
};

