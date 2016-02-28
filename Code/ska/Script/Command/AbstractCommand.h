#pragma once
#include <sstream>
#include "Command.h"


namespace ska {
	class AbstractCommand : public Command
	{
	private:
		std::string interpretSubCommands(std::string& line, IScript* script);

	public:
		AbstractCommand();
		std::string process(IScript* script, std::stringstream& streamCmd) override;
		virtual ~AbstractCommand();

	protected:
		virtual std::string process(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args) = 0;
		virtual char getSeparator() = 0;
	};
}
