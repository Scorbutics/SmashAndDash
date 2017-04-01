#pragma once
#include "Scene.h"
#include "SceneHolder.h"
#include "../../Draw/IGraphicSystem.h"
#include "../../ECS/ISystem.h"

namespace ska {
	class Window;
	class InputContextManager;

	template <class EM, class ED>
	class SceneBase : public Scene {
	public:
		SceneBase(EM& em, ED& ed, Window& w, InputContextManager& ril, SceneHolder& sh) :
			Scene(sh),
			m_entityManager(em), 
			m_eventDispatcher(ed), 
			m_window(w), 
			m_inputCManager(ril) {
			
		}
		
		SceneBase(EM& em, ED& ed, Window& w, InputContextManager& ril, Scene& oldScene) :
			Scene(oldScene), 
			m_entityManager(em),
			m_eventDispatcher(ed),
			m_window(w), 
			m_inputCManager(ril) {
			
		}

		virtual void graphicUpdate(DrawableContainer& drawables) override {
			/* Graphics */
			for (auto& s : m_graphics) {
				s->setDrawables(drawables);
				s->update();
			}
		}

		virtual void eventUpdate(unsigned int) override {
			/* Logics */
			for (auto& s : m_logics) {
				s->update();
			}
		}

		template<class SC, class ... Args>
		void makeNextScene(Args&&... args) {
			m_holder.nextScene(std::make_unique<SC>(m_entityManager, m_eventDispatcher, m_window, m_inputCManager, *this, std::forward<Args>(args)...));
			m_holder.update();
		}

		virtual ~SceneBase() = default;

		
	private:
		std::vector<std::unique_ptr<ISystem>> m_logics;
		std::vector<std::unique_ptr<IGraphicSystem>> m_graphics;

	protected:
		template<class S, class ...Args>
		S* addLogic(Args&& ... args) {
			auto s = std::make_unique<S>(m_entityManager, std::forward<Args>(args)...);
			S* result = static_cast<S*>(s.get());
			m_logics.push_back(std::move(s));
			return result;
		}

		template<class S, class ...Args>
		std::unique_ptr<S> createLogic(Args&&... args){
			return std::make_unique<S>(m_entityManager, std::forward<Args>(args)...);
		}

		template<class S, class ...Args>
		S* addGraphic(Args&& ... args) {
			auto s = std::make_unique<S>(m_entityManager, std::forward<Args>(args)...);
			S* result = static_cast<S*>(s.get());
			m_graphics.push_back(std::move(s));
			return result;
		}

		EM& m_entityManager;
		ED& m_eventDispatcher;
		Window& m_window;
		InputContextManager& m_inputCManager;
	};
}
