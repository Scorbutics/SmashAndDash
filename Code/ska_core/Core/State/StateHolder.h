#pragma once
#include "State.h"
#include "../../Task/TaskQueue.h"

namespace ska {
	class StateHolder : public TaskQueue {
	public:
		StateHolder() = default;
		StateHolder(const StateHolder&) = delete;
		StateHolder& operator=(const StateHolder&) = delete;
		virtual ~StateHolder() = default;

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
