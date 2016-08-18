#pragma once
#include "../../ska/Scene/SceneSwitcher.h"

class WorldScene;

class SceneToMapSwitcher : public ska::SceneSwitcher<WorldScene&> {
public:
	SceneToMapSwitcher(const std::string& map, const std::string& chipset);
	virtual ~SceneToMapSwitcher() = default;
	void switchTo(ska::Window& w, ska::SceneHolder& holder, ska::Scene& lastScene, ska::InputContextManager& icm, WorldScene& ws) const override;
};