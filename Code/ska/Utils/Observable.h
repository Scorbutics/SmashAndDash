#pragma once

#include <vector>
#include <algorithm>
#include "Observer.h"

namespace ska {

	template <class T, class ...Args>
	class Observable
	{
	public:
		Observable() : m_head(nullptr) {

		}

		void addObserver(Observer<T, Args...>& obs) {
			if (m_head != nullptr) {
				m_head->m_left = &obs;
			}
			obs.m_left = nullptr;
			obs.m_right = m_head;		
			m_head = &obs;
		}

		void removeObserver(Observer<T, Args...>& obs) {
			if (m_head == &obs) {
				m_head = obs.m_right;
			}

			if (obs.m_left != nullptr) {
				obs.m_left->m_right = obs.m_right;
			}

			if (obs.m_right != nullptr) {
				obs.m_right->m_left = obs.m_left;
			}
		}

		bool notifyObservers(T& t, Args... args) {
			bool result = true;
			for (auto it = m_head; it != nullptr; it = it->m_right) {
				result |= it->receive(t, std::forward<Args&>(args)...);
			}
			return result;
		}


		virtual ~Observable() = default;

	private:
		Observer<T, Args...>* m_head;
	};
}

