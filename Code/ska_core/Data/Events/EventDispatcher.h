#pragma once

#include "../../Utils/Observable.h"

namespace ska {

	template <class ...ET>
	class EventDispatcher : public Observable<ET>... {
	public:
		EventDispatcher() = default;
		EventDispatcher(const EventDispatcher<ET...>&) = delete;

		template <class ...ET2>
		void addMultipleObservers(Observer<ET2>&... obs) {
			int _[] = { 0, (Observable<ET2>::addObserver(obs), 0)... };
			(void)_;
		}

		template <class ...ET2>
		void removeMultipleObservers(Observer<ET2>&... obs) {
			int _[] = { 0, (Observable<ET2>::removeObserver(obs), 0)... };
			(void)_;
		}

		virtual ~EventDispatcher() = default;
	};

}
