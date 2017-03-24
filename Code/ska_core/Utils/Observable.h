#pragma once

#include <vector>
#include <unordered_set>
#include "Observer.h"

namespace ska {

	template <class T, class ...Args>
	class Observable
	{
	public:
		Observable() {
		}

		void addObserver(Observer<T, Args...>& obs) {
			m_head.emplace(&obs);
		}

		void removeObserver(Observer<T, Args...>& obs) {
			m_head.erase(&obs);
		}

		bool notifyObservers(T& t, Args... args) {
			auto hasBeenHandled = false;
			for (auto& obs : m_head) {
				hasBeenHandled |= obs->receive(t, std::forward<Args&>(args)...);
			}
			return hasBeenHandled;
		}


		virtual ~Observable() = default;

	private:
		std::unordered_set<Observer<T, Args...>*> m_head;
	};
}

