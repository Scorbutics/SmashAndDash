#pragma once
#include <fstream>
#include <memory>
#include <unordered_map>


class Command
{
public:

	Command(){}
	virtual bool process(const std::string& extendedName, std::stringstream& streamCmd, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) = 0;
	virtual ~Command(){}


};
typedef std::unique_ptr<Command> Command_ptr;