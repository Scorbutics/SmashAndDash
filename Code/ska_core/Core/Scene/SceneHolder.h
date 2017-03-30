#pragma once
#include "SceneType.h"
#include "Scene.h"
#include "../../Task/TaskQueue.h"

namespace ska {
	class SceneHolder : public TaskQueue {
	public:
		SceneHolder() = default;
		SceneHolder(const SceneHolder&) = delete;
		SceneHolder& operator=(const SceneHolder&) = delete;
		~SceneHolder() = default;

		virtual void update() = 0;
		virtual void nextScene(ScenePtr& scene) = 0;
		virtual ScenePtr& getScene() = 0;

	protected:
		ScenePtr m_currentScene;
		ScenePtr m_nextScene;
	};
}
