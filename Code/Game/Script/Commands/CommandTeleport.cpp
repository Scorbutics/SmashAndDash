#include "CommandTeleport.h"
#include "Script/ScriptComponent.h"
#include "Script/System/ScriptAutoSystem.h"
#include "Exceptions/InvalidPathException.h"
#include "World/TileWorld.h"
#include "../../Utils/IDs.h"
#include "Utils/StringUtils.h"
#include "../../Gameplay/Data/MapEvent.h"
#include "World/TileWorldLoaderAggregate.h"
#include "World/LayerEventLoaderText.h"
#include "World/LayerLoaderImage.h"
#include "Utils/FileUtils.h"
#include "ECS/EntityLocator.h"
#include "../ScriptConstants.h"

CommandTeleport::CommandTeleport(const ska::TileWorld& w, const ska::EntityLocator& locator, ska::EntityManager& entityManager, PokemonGameEventDispatcher& ged) :
	AbstractFunctionCommand(entityManager),
	m_world(w),
	m_ged(ged),
	m_locator(locator) {
}

int CommandTeleport::argumentsNumber() {
	return 3;
}

std::string CommandTeleport::execute(ska::ScriptComponent& script, ska::MemoryScript& memoryScript, const std::vector<std::string>& args) {
	const auto& mapName = args[0];
	const auto x = ska::StringUtils::strToInt(args[1]);
	const auto y = ska::StringUtils::strToInt(args[2]);

	const auto buf = "./Levels/" + mapName + "/" + mapName + ".ini";
	const auto mapReader = ska::IniReader{ buf };
	const auto& chipsetName = mapReader.get<std::string>("Chipset file");

	MapEvent me(MapEventType::Switch_Map);
	me.chipsetName = chipsetName;
	me.mapName = mapName;
	me.position.x = m_world.getBlockSize() * x;
	me.position.y = m_world.getBlockSize() * y;
	m_ged.ska::Observable<MapEvent>::notifyObservers(me);
	
	return "";
}
