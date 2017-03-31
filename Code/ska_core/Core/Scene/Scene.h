#pragma once
#include <vector>
#include <memory>
#include "HasGraphic.h"
#include "HasLogic.h"

namespace ska {
	class SceneHolder;
	class ISystem;
	class IGraphicSystem;

	class Scene : public HasGraphic, public HasLogic {
	public:
		explicit Scene(SceneHolder& sh);
		Scene(Scene& oldScene);

		void operator=(const Scene&) = delete;

		virtual void load(std::unique_ptr<Scene>* lastScene) = 0;
		virtual bool unload() = 0;
		virtual void graphicUpdate(DrawableContainer& drawables) override;
		virtual void eventUpdate(unsigned int ellapsedTime) override;

		virtual ~Scene() = default;

	protected:
		SceneHolder& m_holder;

		//TODO encapsulation + forte
		std::vector<ISystem*> m_logics;
		std::vector<IGraphicSystem*> m_graphics;

	};

	using ScenePtr = std::unique_ptr<Scene>;
}

