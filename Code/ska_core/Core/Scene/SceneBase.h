#pragma once
#include "Scene.h"
#include "SceneHolder.h"

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

		template<class SC, class ... Args>
		void makeNextScene(Args&&... args) {
			m_holder.nextScene(std::make_unique<SC>(m_entityManager, m_eventDispatcher, m_window, m_inputCManager, *this, std::forward<Args>(args)...));
			m_holder.update();
		}

		virtual ~SceneBase() = default;

	protected:
		EM& m_entityManager;
		ED& m_eventDispatcher;
		Window& m_window;
		InputContextManager& m_inputCManager;
	};
}
