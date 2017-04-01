#include <utility>

#include "ScriptBasicCommandsSystem.h"

#include "../Command/CommandEnd.h"
#include "../Command/CommandCalculate.h"
#include "../Command/CommandAssignSwitch.h"
#include "../Command/CommandEndScript.h"
#include "../Command/CommandAssign.h"
#include "../Command/CommandRandom.h"
#include "../Command/CommandWait.h"
#include "../Command/CommandLog.h"
#include "../Command/CommandScript.h"
#include "../Command/CommandIf.h"
#include "../Command/CommandElse.h"
#include "../Command/CommandElseEnd.h"
#include "../Command/CommandRemoveComponent.h"
#include "../Command/CommandRestoreComponent.h"
#include "../Command/CommandBlockAuthorize.h"

ska::ScriptBasicCommandsSystem::ScriptBasicCommandsSystem(EntityManager& entityManager, World& w, Savegame& saveGame) : 
	ScriptAutoSystem(entityManager, w, BasicScriptCommandHelper(w, entityManager), saveGame) {

}

ska::ScriptBasicCommandsSystem::ScriptBasicCommandsSystem(EntityManager& entityManager, World& w, const ScriptCommandHelper& sch, Savegame& saveGame) : 
	ScriptAutoSystem(entityManager, w, sch, saveGame) {

}

void ska::ScriptBasicCommandsSystem::BasicScriptCommandHelper::setupCommands(World&, std::unordered_map<std::string, CommandPtr>& c) const {
	c[CommandEnd::getCmdName()] = std::move(CommandPtr(new CommandEnd(m_entityManager)));
	c[ControlStatement::getCommandIf()] = std::move(CommandPtr(new CommandIf(m_entityManager)));
	c[ControlStatement::getCommandElse()] = std::move(CommandPtr(new CommandElse(m_entityManager)));
	c[ControlStatement::getCommandEndIf()] = std::move(CommandPtr(new CommandElseEnd(m_entityManager)));
	c["end_script"] = std::move(CommandPtr(new CommandEndScript(m_entityManager)));
	c["calculate"] = std::move(CommandPtr(new CommandCalculate(m_entityManager)));
	c["assign"] = std::move(CommandPtr(new CommandAssign(m_entityManager)));
	c["assign_switch"] = std::move(CommandPtr(new CommandAssignSwitch(m_entityManager)));
	c["random"] = std::move(CommandPtr(new CommandRandom(m_entityManager)));
	c["log"] = std::move(CommandPtr(new CommandLog(m_entityManager)));
	c["script"] = std::move(CommandPtr(new CommandScript(m_entityManager)));
	c["wait"] = std::move(CommandPtr(new CommandWait(m_entityManager)));
	c["remove_component"] = std::move(CommandPtr(new CommandRemoveComponent(m_entityManager)));
	c["restore_component"] = std::move(CommandPtr(new CommandRestoreComponent(m_entityManager)));
	c["block_authorize"] = std::move(CommandPtr(new CommandBlockAuthorize(m_entityManager)));
}


ska::ScriptBasicCommandsSystem::~ScriptBasicCommandsSystem() {
}
