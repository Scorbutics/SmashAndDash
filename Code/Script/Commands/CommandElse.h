#pragma once
#include "ControlStatement.h"
class CommandElse :
	public ControlStatement
{
public:
	CommandElse();
	virtual ~CommandElse();

	virtual std::string analyzeLine(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::ifstream& fscript) override; 
	virtual const std::string& getCmdName() override;
};

