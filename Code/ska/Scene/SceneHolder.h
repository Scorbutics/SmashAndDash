#pragma once
#include "SceneType.h"
#include "Scene.h"

namespace ska {
	class SceneHolder {
	public:
		SceneHolder() = default;
		virtual ~SceneHolder() = default;

		virtual void nextScene(ScenePtr& scene) = 0;

	protected:
		ScenePtr m_currentScene;
	};
}