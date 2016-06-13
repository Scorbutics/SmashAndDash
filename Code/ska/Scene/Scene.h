#pragma once
#include <memory>
#include "HasGraphic.h"
#include "HasLogic.h"

namespace ska {
	class SceneHolder;
	class InputContextManager;
	class ISystem;
	class IGraphicSystem;
	class Scene : public HasGraphic, public HasLogic
	{
	public:
		Scene(SceneHolder& sh, InputContextManager& ril);
		Scene(Scene& oldScene);

		virtual void load(std::unique_ptr<Scene>* lastScene) = 0;
		virtual bool unload() = 0;
		virtual void graphicUpdate(DrawableContainer& drawables) override;
		virtual void eventUpdate(bool movingDisallowed) override;

		virtual ~Scene() = default;

	protected:
		InputContextManager& m_inputCManager;
		SceneHolder& m_holder;
		std::vector<ska::ISystem*> m_logics;
		std::vector<ska::IGraphicSystem*> m_graphics;

	};

	using ScenePtr = std::unique_ptr<Scene>;
}

