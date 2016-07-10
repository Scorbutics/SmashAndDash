#pragma once
#include "ScriptBasicCommandsSystem.h"

#include "../Command\CommandEnd.h"
#include "../Command\CommandCalculate.h"
#include "../Command/CommandAssignSwitch.h"
#include "../Command\CommandEndScript.h"
#include "../Command\CommandAssign.h"
#include "../Command\CommandRandom.h"
#include "../Command\CommandWait.h"
#include "../Command/CommandLog.h"
#include "../Command/CommandScript.h"
#include "../Command/CommandIf.h"
#include "../Command/CommandElse.h"
#include "../Command/CommandElseEnd.h"
#include "../Command/CommandRemoveComponent.h"
#include "../Command/CommandRestoreComponent.h"
#include "../Command/CommandBlockAuthorize.h"

ska::ScriptBasicCommandsSystem::ScriptBasicCommandsSystem(EntityManager& entityManager, ska::Savegame& saveGame) : ScriptAutoSystem(BasicScriptCommandHelper(entityManager), entityManager, saveGame) {

}

ska::ScriptBasicCommandsSystem::ScriptBasicCommandsSystem(const ScriptCommandHelper& sch, EntityManager& entityManager, ska::Savegame& saveGame) : ScriptAutoSystem(sch, entityManager, saveGame) {

}

void ska::ScriptBasicCommandsSystem::BasicScriptCommandHelper::setupCommands(std::unordered_map<std::string, CommandPtr>& c) const {
	c[CommandEnd::getCmdName()] = std::move(ska::CommandPtr(new CommandEnd(m_entityManager)));
	c[ControlStatement::getCommandIf()] = std::move(ska::CommandPtr(new CommandIf(m_entityManager)));
	c[ControlStatement::getCommandElse()] = std::move(ska::CommandPtr(new CommandElse(m_entityManager)));
	c[ControlStatement::getCommandEndIf()] = std::move(ska::CommandPtr(new CommandElseEnd(m_entityManager)));
	c["end_script"] = std::move(ska::CommandPtr(new CommandEndScript(m_entityManager)));
	c["calculate"] = std::move(ska::CommandPtr(new CommandCalculate(m_entityManager)));
	c["assign"] = std::move(ska::CommandPtr(new CommandAssign(m_entityManager)));
	c["assign_switch"] = std::move(ska::CommandPtr(new CommandAssignSwitch(m_entityManager)));
	c["random"] = std::move(ska::CommandPtr(new CommandRandom(m_entityManager)));
	c["log"] = std::move(ska::CommandPtr(new CommandLog(m_entityManager)));
	c["script"] = std::move(ska::CommandPtr(new CommandScript(m_entityManager)));
	c["wait"] = std::move(ska::CommandPtr(new CommandWait(m_entityManager)));
	c["remove_component"] = std::move(ska::CommandPtr(new CommandRemoveComponent(m_entityManager)));
	c["restore_component"] = std::move(ska::CommandPtr(new CommandRestoreComponent(m_entityManager)));
	c["block_authorize"] = std::move(ska::CommandPtr(new CommandBlockAuthorize(m_entityManager)));
}


ska::ScriptBasicCommandsSystem::~ScriptBasicCommandsSystem() {
}
