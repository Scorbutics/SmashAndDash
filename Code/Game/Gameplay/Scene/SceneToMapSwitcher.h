#pragma once
#include "Core/Scene/SceneBase.h"
#include "../World/WorldScene.h"

class SceneMap;

class SceneToMapSwitcher {
public:
	SceneToMapSwitcher(const std::string& map, const std::string& chipsetName, WorldScene& ws) :
		m_ws(ws),
		m_mapName(map),
		m_chipsetName(chipsetName) {}
	SceneToMapSwitcher& operator=(const SceneToMapSwitcher&) = delete;
	~SceneToMapSwitcher() = default;

	template<class EM, class ED>
	void switchTo(ska::SceneBase<EM, ED>& lastScene) const {
		lastScene.template makeNextSceneAndTransmitLinkedSubscenes<ska::SceneBase<EM, ED>, SceneMap>(lastScene, m_ws, m_mapName, m_chipsetName, m_ws.getFileName() == m_mapName);
	}

protected:
	WorldScene& m_ws;
	const std::string m_mapName;
	const std::string m_chipsetName;
};

