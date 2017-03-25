#include <iostream>
#include "AbstractCommand.h"
#include "Utils/StringUtils.h"
#include "../ScriptUtils.h"
#include "../ScriptSymbolsConstants.h"
#include "../System/ScriptAutoSystem.h"
#include "../ScriptComponent.h"

ska::AbstractCommand::AbstractCommand(EntityManager& entityManager) : Command(entityManager) {
}

std::string ska::AbstractCommand::process(ScriptAutoSystem& system, ScriptComponent& script, std::stringstream& streamCmd) {

	std::string line;
	getline(streamCmd, line);

	line = StringUtils::trim(line);

	/* Avant tout traitement, effectue toutes les exécutions des sous-commandes possibles */
	interpretSubCommands(system, line, script);

	std::vector<std::string> args = StringUtils::split(line, getSeparator());

	for (std::string& arg : args) {
		arg = StringUtils::trim(arg);
		/* Pour chaque argument, explicite les valeurs des variables */
		arg = ScriptUtils::getValueFromVarOrSwitchNumber(system.getSavegame(), script, arg);
	}


	return process(script, streamCmd, args);
}

std::string ska::AbstractCommand::interpretSubCommands(ScriptAutoSystem& system, std::string& line, ScriptComponent& script) {
	size_t outputCommandSize = 0;
	size_t offset;
	std::string parsedArg;

	/* Conservation de tout ce qui se trouve sur la ligne avant l'appel à la sous-commande */
	size_t offsetLineCmd = line.find_first_of(ScriptSymbolsConstants::METHOD);
	std::string firstLinePart = (offsetLineCmd == std::string::npos ? "" : line.substr(0, offsetLineCmd));
	parsedArg += firstLinePart;

	do {
		std::string expression = line.substr(outputCommandSize);

		parsedArg += ScriptUtils::getFirstExpressionFromLine(system, expression, script, &offset);
		parsedArg += " ";
		outputCommandSize += offset;
	} while (offset != 0 && outputCommandSize < line.size());
	line = StringUtils::rtrim(parsedArg);
	return "";
}

ska::AbstractCommand::~AbstractCommand()
{
}
