#include "CommandCalculate.h"
#include "../../Utils\StringUtils.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Utils\FormalCalculation\FormalCalculator.h"
#include "../ScriptDispatcher.h"

using namespace std;

ska::CommandCalculate::CommandCalculate()
{
}


ska::CommandCalculate::~CommandCalculate()
{
}

int ska::CommandCalculate::argumentsNumber() {
	return 1;
}

std::string ska::CommandCalculate::execute(IScript* script, std::vector<std::string>& args)
{
	float f = FormalCalculator::interpretFormalCalculation(ScriptUtils::replaceVariablesByNumerics(script->getParent().getSavegame(), script->getExtendedName(), args[0], script->getVarMap()));
	return ska::StringUtils::intToStr((int)f);
}
