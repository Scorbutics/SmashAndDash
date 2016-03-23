#include "CommandCalculate.h"
#include "../../Utils\StringUtils.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Utils\FormalCalculation\FormalCalculator.h"
#include "../ScriptDispatcher.h"
#include "../System/ScriptAutoSystem.h"

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

std::string ska::CommandCalculate::execute(ScriptComponent& script, std::vector<std::string>& args)
{
	float f = FormalCalculator::interpretFormalCalculation(ScriptUtils::replaceVariablesByNumerics(script.parent->getSavegame(), script.extendedName, args[0], script.varMap));
	return ska::StringUtils::intToStr((int)f);
}
