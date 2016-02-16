#pragma once
#include "ControlStatement.h"


class CommandElseEnd : public ControlStatement {

public:
	virtual const std::string& getCmdName() {
		return ControlStatement::getCommandEndIf();
	}

protected:
	virtual bool analyzeLine(const std::string& extendedName, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override
	{
		return true;
	}

	virtual int argumentsNumber() {
		return 0;
	}
};