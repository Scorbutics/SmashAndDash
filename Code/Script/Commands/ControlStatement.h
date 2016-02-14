#pragma once
#include "AbstractCommand.h"
class ControlStatement :
	public AbstractCommand
{
public:
	ControlStatement();
	virtual ~ControlStatement();

protected:
	virtual bool process(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result);
	virtual char getSeparator();

	virtual bool analyzeLine(const std::string& extendedName, std::string& line, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) = 0;
};
