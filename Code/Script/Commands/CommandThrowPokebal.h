#pragma once
#include "AbstractCommand.h"
class CommandThrowPokebal :
	public AbstractCommand
{
public:
	CommandThrowPokebal();
	virtual ~CommandThrowPokebal();

	virtual bool execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override;
	virtual int argumentsNumber();
};

