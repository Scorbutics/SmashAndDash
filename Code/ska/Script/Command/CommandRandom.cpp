#include "CommandRandom.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Utils\StringUtils.h"

using namespace std;

ska::CommandRandom::CommandRandom(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


ska::CommandRandom::~CommandRandom()
{
}

int ska::CommandRandom::argumentsNumber() {
	return 1;
}

std::string ska::CommandRandom::execute(ScriptComponent& script, std::vector<std::string>& args) {
	string s, value, commandCall;
	int valueInt, resultInt;

	value = args[0];
	valueInt = ska::StringUtils::strToInt(value);

	if (valueInt > 0)
	{
		resultInt = rand() % valueInt;
		return ska::StringUtils::intToStr(resultInt);
	}
	return "";
}
