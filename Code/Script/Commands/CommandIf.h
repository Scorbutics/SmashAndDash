#pragma once
#include "ControlStatement.h"
class CommandIf :
	public ControlStatement
{
public:
	CommandIf();
	virtual ~CommandIf();

	virtual bool analyzeLine(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override;
};

