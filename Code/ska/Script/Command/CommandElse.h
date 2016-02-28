#pragma once
#include "ControlStatement.h"
namespace ska {
	class CommandElse :
		public ControlStatement
	{
	public:
		CommandElse();
		virtual ~CommandElse();

		virtual std::string analyzeLine(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args) override;
		virtual const std::string& getCmdName() override;
	};
}
