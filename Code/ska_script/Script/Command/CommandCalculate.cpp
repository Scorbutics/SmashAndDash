#include "CommandCalculate.h"
#include "Utils/StringUtils.h"
#include "../ScriptUtils.h"
#include "Utils/FormalCalculation/FormalCalculator.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandCalculate::CommandCalculate(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
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
	float f = FormalCalculator::interpretFormalCalculation(ScriptUtils::replaceVariablesByNumerics(script.parent->getSavegame(), script, args[0]));
	return StringUtils::intToStr((int)f);
}
