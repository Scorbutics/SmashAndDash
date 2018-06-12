#include "ScriptCommandsSystem.h"
#include "../Commands/CommandAddPokemon.h"
#include "../Commands/CommandChoice.h"
#include "../Commands/CommandCinematic.h"
#include "../Commands/CommandDirection.h"
#include "../Commands/CommandFollow.h"
#include "../Commands/CommandHeal.h"
#include "../Commands/CommandHideCharacter.h"
#include "../Commands/CommandHideGUI.h"
#include "../Commands/CommandKillEntity.h"
#include "../Commands/CommandMessage.h"
#include "../Commands/CommandMove.h"
#include "../Commands/CommandPlayAnimation.h"
#include "../Commands/CommandPlayCrumbling.h"
#include "../Commands/CommandPlayerPresence.h"
#include "../Commands/CommandPlayShaking.h"
#include "../Commands/CommandPokemonOut.h"
#include "../Commands/CommandShop.h"
#include "../Commands/CommandStop.h"
#include "../Commands/CommandStuckCharacter.h"
#include "../Commands/CommandTeleport.h"
#include "../Commands/CommandThrowPokebal.h"
#include "../Commands/CommandTranslationCamera.h"
#include "../Commands/CommandExpulse.h"
#include "../Commands/CommandJump.h"

ScriptCommandsSystem::ScriptCommandsSystem(ska::EntityManager& entityManager, const ska::EntityLocator& locator, ska::TileWorld& w, ska::MemoryScript& saveGame, PokemonGameEventDispatcher& ged) :
	ScriptBasicCommandsSystem(w, entityManager, ged, ScriptCommandHelper(w, entityManager, locator, ged), saveGame) {
}

void ScriptCommandsSystem::ScriptCommandHelper::setupCommands(std::unordered_map<std::string, ska::CommandPtr>& c) const {
	BasicScriptCommandHelper::setupCommands(c);
	c["move"] =  std::make_unique<CommandMove>(m_entityManager);
	c["message"] =  std::make_unique<CommandMessage>(m_ged, m_entityManager);
	c["choice"] =  std::make_unique<CommandChoice>(m_entityManager);
	c["heal"] =  std::make_unique<CommandHeal>(m_entityManager);
	c["stop"] =  std::make_unique<CommandStop>(m_entityManager);
	c["direction"] =  std::make_unique<CommandDirection>(m_entityManager);
	c["animation"] =  std::make_unique<CommandPlayAnimation>(m_entityManager);
	c["crumbling"] =  std::make_unique<CommandPlayCrumbling>(m_entityManager);
	c["shaking"] =  std::make_unique<CommandPlayShaking>(m_entityManager);
	c["camera_translation"] =  std::make_unique<CommandTranslationCamera>(m_entityManager);
	c["stuck_char"] =  std::make_unique<CommandStuckCharacter>(m_entityManager);
	c["hide_char"] =  std::make_unique<CommandHideCharacter>(m_entityManager);
	c["follow_char"] =  std::make_unique<CommandFollow>(m_entityManager);
	c["shop"] =  std::make_unique<CommandShop>(m_entityManager);
	c["hide_gui"] =  std::make_unique<CommandHideGUI>(m_entityManager);
	c["teleport"] =  std::make_unique<CommandTeleport>(m_world, m_locator, m_entityManager, m_ged);
	c["cinematic"] =  std::make_unique<CommandCinematic>(m_entityManager);
	c["pokeball"] =  std::make_unique<CommandThrowPokebal>(m_entityManager);
	c["kill"] =  std::make_unique<CommandKillEntity>(m_entityManager);
	c["add_pokemon"] =  std::make_unique<CommandAddPokemon>(m_entityManager);
	c["throw_pokemon"] =  std::make_unique<CommandPokemonOut>(m_entityManager);
	c["player_presence"] =  std::make_unique<CommandPlayerPresence>(m_entityManager);
	c["expulse"] =  std::make_unique<CommandExpulse>(m_world, m_entityManager);
	c["jump"] =  std::make_unique<CommandJump>(m_entityManager);
	//c["block_collision"] = move(ska::CommandPtr(new CommandBlockCollision(m_entityManager));
}

