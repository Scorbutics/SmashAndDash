#pragma once
#include "ControlStatement.h"
class CommandElseEnd : public ControlStatement {


	virtual bool analyzeLine(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override
	{
		return true;
	}
	
	virtual int argumentsNumber() {
		return 0;
	}
};