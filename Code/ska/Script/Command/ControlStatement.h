#pragma once
#include "AbstractCommand.h"

namespace ska {
	class ControlStatement : public AbstractCommand {
	public:
		ControlStatement();
		virtual ~ControlStatement();

		static const std::string& getCommandEndIf();
		static const std::string& getCommandElse();
		static const std::string& getCommandIf();

	protected:
		virtual std::string process(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args) override;
		virtual char getSeparator() override;

		virtual std::string analyzeLine(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args) = 0;
		virtual const std::string& getCmdName() = 0;

	};
}