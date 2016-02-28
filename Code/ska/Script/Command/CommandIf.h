#pragma once
#include "ControlStatement.h"
namespace ska {
	class CommandIf : public ControlStatement
	{
	public:
		CommandIf();
		virtual ~CommandIf();

		virtual std::string analyzeLine(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args) override;
		virtual const std::string& getCmdName() override;
	};
}
