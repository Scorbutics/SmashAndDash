#pragma once
#include <memory>
#include "HasGraphic.h"
#include "HasLogic.h"

namespace ska {
	class SceneHolder;
	class InputContextManager;
	class ISystem;
	class IGraphicSystem;

	class Scene : public HasGraphic, public HasLogic {
	public:
		Scene(SceneHolder& sh, InputContextManager& ril);
		Scene(Scene& oldScene);

		void operator=(const Scene&) = delete;

		virtual void load(std::unique_ptr<Scene>* lastScene) = 0;
		virtual bool unload() = 0;
		virtual void graphicUpdate(DrawableContainer& drawables) override;
		virtual void eventUpdate(unsigned int ellapsedTime) override;

		virtual ~Scene() = default;

	protected:
		InputContextManager& m_inputCManager;
		SceneHolder& m_holder;
		std::vector<ISystem*> m_logics;
		std::vector<IGraphicSystem*> m_graphics;

	};

	using ScenePtr = std::unique_ptr<Scene>;
}

