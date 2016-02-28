#pragma once
#include "AbstractCommand.h"
namespace ska {
	class AbstractFunctionCommand :
		public AbstractCommand
	{
	public:
		AbstractFunctionCommand();
		virtual ~AbstractFunctionCommand();

	protected:
		virtual int argumentsNumber() = 0;
		virtual std::string execute(IScript* script, std::vector<std::string>& args) = 0;

		std::string process(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args) override;
		virtual char getSeparator() override;
	};
}
