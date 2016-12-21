#pragma once
#include "../../ska/Scene/SceneSwitcher.h"
#include "../../ska/Core/GameEventDispatcher.h"

class WorldScene;

class SceneToMapSwitcher : public ska::SceneSwitcher<WorldScene&> {
public:
	SceneToMapSwitcher(const std::string& map, const std::string& chipset, ska::GameEventDispatcher& ged);
	virtual ~SceneToMapSwitcher() = default;
	void switchTo(ska::Window& w, ska::SceneHolder& holder, ska::Scene& lastScene, ska::InputContextManager& icm, WorldScene& ws) const override;
private:
	ska::GameEventDispatcher& m_ged;
};