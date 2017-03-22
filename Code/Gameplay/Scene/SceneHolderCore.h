#pragma once
#include "../../ska/Scene/SceneHolder.h"

class SceneHolderCore : public ska::SceneHolder {
public:
	SceneHolderCore() = default;
	~SceneHolderCore() = default;

	void update() override;
	void nextScene(std::unique_ptr<ska::Scene>& scene) override;
	ska::ScenePtr& getScene() override;
	
};
