#pragma once
#include "CommandIf.h"
class CommandElsif :
	public CommandIf
{
public:
	CommandElsif();
	virtual ~CommandElsif();
	virtual const std::string& getCmdName() override;
};

