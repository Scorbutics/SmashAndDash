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

bool CommandCalculate::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	float f = FormalCalculator::interpretFormalCalculation(ScriptUtils::replaceVariablesByNumerics(extendedName, args[0], varMap));
	if (result != NULL)
		*result = StringUtils::intToStr((int)f);
	return true;
}
