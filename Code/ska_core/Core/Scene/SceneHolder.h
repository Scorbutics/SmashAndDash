#pragma once
#include "Scene.h"
#include "../../Task/TaskQueue.h"

namespace ska {
	class SceneHolder : public TaskQueue {
	public:
		SceneHolder() = default;
		SceneHolder(const SceneHolder&) = delete;
		SceneHolder& operator=(const SceneHolder&) = delete;
		virtual ~SceneHolder() = default;

		virtual void update() = 0;

		template<class S>
		void nextScene(std::unique_ptr<S>&& scene) {
			m_nextScene = std::move(scene);
		}

		/*ScenePtr& getScene() {
			return m_currentScene;
		}*/

	protected:
		ScenePtr m_nextScene;
	};
}
