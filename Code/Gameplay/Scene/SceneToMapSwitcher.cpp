#include "SceneToMapSwitcher.h"
#include "SceneMap.h"
#include "../World/WorldScene.h"
#include "../../ska/Graphic/GUI/Window.h"
#include "../../ska/Scene/SceneHolder.h"

SceneToMapSwitcher::SceneToMapSwitcher(const std::string& map, const std::string& chipset, PokemonGameEventDispatcher& ged) :
SceneSwitcher(map, chipset),
m_ged(ged) {
}

void SceneToMapSwitcher::switchTo(ska::Window& w, ska::SceneHolder& holder, ska::Scene& lastScene, ska::InputContextManager&, WorldScene& ws) const {
	auto scene = ska::ScenePtr(new SceneMap(w, m_ged, lastScene, ws, m_mapName, m_chipsetName, ws.getFileName() == m_mapName));
	holder.nextScene(scene);
}
