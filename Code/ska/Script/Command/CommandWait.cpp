#include "CommandWait.h"
#include "../../Utils/StringUtils.h"
#include "../../Script/IScript.h"

ska::CommandWait::CommandWait()
{
}


ska::CommandWait::~CommandWait()
{
}

int ska::CommandWait::argumentsNumber() {
	return 1;
}

std::string ska::CommandWait::execute(ska::IScript* script, std::vector<std::string>& args)
{
	unsigned int duree = 0, t0 = 0;
	duree = ska::StringUtils::strToInt(args[0]);

	/* Delay the script : assign an uint value > 1 */
	script->delay(duree);
	return "";
}
