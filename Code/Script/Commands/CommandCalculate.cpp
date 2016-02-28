#include "CommandCalculate.h"
#include "../../Utils\StringUtils.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Utils\FormalCalculation\FormalCalculator.h"

using namespace std;

CommandCalculate::CommandCalculate()
{
}


CommandCalculate::~CommandCalculate()
{
}

int CommandCalculate::argumentsNumber() {
	return 1;
}

std::string CommandCalculate::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	float f = FormalCalculator::interpretFormalCalculation(ScriptUtils::replaceVariablesByNumerics(script->getExtendedName(), args[0], script->getVarMap()));
	return ska::StringUtils::intToStr((int)f);
}
