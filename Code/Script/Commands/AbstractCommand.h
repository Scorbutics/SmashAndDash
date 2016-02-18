#pragma once
#include <sstream>
#include "Command.h"



class AbstractCommand : public Command
{
private:
	std::string interpretSubCommands(std::string& line, IScript* script, std::ifstream& fscript);

public:
	AbstractCommand();
	std::string process(IScript* script, std::stringstream& streamCmd, std::ofstream& scriptList, std::ifstream& fscript);
	virtual ~AbstractCommand();

protected:
	virtual std::string process(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::ifstream& fscript) = 0;
	virtual char getSeparator() = 0;
};

