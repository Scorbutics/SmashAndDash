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

		//TODO vers SceneBase et template puis modifier les vector avec des unique_ptr
		void addLogic(ISystem& s){
			m_logics.push_back(&s);
		}

		void addGraphic(IGraphicSystem& s){
			m_graphics.push_back(&s);
		}

	private:
		std::vector<ISystem*> m_logics;
		std::vector<IGraphicSystem*> m_graphics;

	};

	using ScenePtr = std::unique_ptr<Scene>;
}

