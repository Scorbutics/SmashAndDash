#pragma once
#include "SceneType.h"
#include "Scene.h"
#include "../Task/TaskQueue.h"

namespace ska {
	class SceneHolder : public TaskQueue {
	public:
		SceneHolder() = default;
		virtual ~SceneHolder() = default;

		virtual void update() = 0;
		virtual void nextScene(ScenePtr& scene) = 0;
		virtual ska::ScenePtr& getScene() = 0;

	protected:
		ScenePtr m_currentScene;
		ScenePtr m_nextScene;
	};
}