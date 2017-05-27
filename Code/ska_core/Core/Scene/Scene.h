#pragma once
#include <memory>
#include "HasGraphic.h"
#include "HasLogic.h"

namespace ska {
	class SceneHolder;
	class ISystem;
	class IGraphicSystem;
	class DrawableContainer;

	class Scene : public HasGraphic, public HasLogic {
	public:
		explicit Scene(SceneHolder& sh);
		Scene(Scene& oldScene);

		void operator=(const Scene&) = delete;

		virtual void graphicUpdate(unsigned ellapsedTime, DrawableContainer& drawables) override = 0;
		virtual void eventUpdate(unsigned int) override = 0;
		virtual void load(std::unique_ptr<Scene>* lastScene) = 0;
		virtual bool unload() = 0;

		virtual ~Scene() = default;

	protected:	
		SceneHolder& m_holder;

	};

	using ScenePtr = std::unique_ptr<Scene>;
}

