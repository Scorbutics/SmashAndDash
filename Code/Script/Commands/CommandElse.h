#pragma once
#include "ControlStatement.h"
class CommandElse :
	public ControlStatement
{
public:
	CommandElse();
	virtual ~CommandElse();

	virtual bool analyzeLine(const std::string& extendedName, std::string& line, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override;
};

