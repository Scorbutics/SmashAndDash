#pragma once
#include "ControlStatement.h"
class CommandIf :
	public ControlStatement
{
public:
	CommandIf();
	virtual ~CommandIf();

	virtual std::string analyzeLine(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual const std::string& getCmdName() override;
};

