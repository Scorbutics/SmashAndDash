#pragma once

namespace ska {
	
	template<class EM, class ED>
	struct StateData {
		StateData(EM& em, ED& ed) :
			m_entityManager(em),
			m_eventDispatcher(ed) {
		}
		EM& m_entityManager;
		ED& m_eventDispatcher;
	};
}
