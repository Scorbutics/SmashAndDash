#pragma once
#include <sstream>
#include "Command.h"



class AbstractCommand : public Command
{
private:
	void parseArgument(std::string& arg, const std::string& extendedName, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result);
	void interpretSubCommands(std::string& line, const std::string& extendedName, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result);

public:
	AbstractCommand();
	bool process(const std::string& extendedName, std::stringstream& streamCmd, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result);
	virtual ~AbstractCommand();

protected:
	virtual bool process(const std::string& extendedName, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) = 0;
	virtual char getSeparator() = 0;
};

