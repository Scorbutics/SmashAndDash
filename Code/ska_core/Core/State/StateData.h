#pragma once
#include "../Window.h"
#include "../../Inputs/InputContextManager.h"

namespace ska {
	
	template<class EM, class ED>
	struct StateData {
		StateData(EM& em, ED& ed, Window& w, InputContextManager& inputCManager) :
			m_entityManager(em),
			m_eventDispatcher(ed),
			m_window(w),
			m_inputCManager(inputCManager) {
		}
		EM& m_entityManager;
		ED& m_eventDispatcher;
		Window& m_window;
		InputContextManager& m_inputCManager;
	};
}
