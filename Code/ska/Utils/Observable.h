#ifndef DEF_OBSERVABLE
#define DEF_OBSERVABLE

#include <string>
#include <vector>

#include "Observer.h"

namespace ska {

	template <class T>
	class Observable
	{
	public:
		void addObserver(Observer<T>* obs) {
			auto temp = std::find(m_observerList.begin(), m_observerList.end(), obs);
			if (temp == m_observerList.end()) {
				m_observerList.push_back(obs);
			}
		}

		void removeObserver(Observer<T>* obs) {
			auto temp = std::find(m_observerList.begin(), m_observerList.end(), obs);
			if (temp != m_observerList.end()) {
				m_observerList.erase(temp);
			}
		}

		void notifyObservers(const EventArg& e, T& t) {
			for (auto &it : m_observerList) {
				it->update(this, e, t);
			}
		}

		void notifyObserver(Observer<T>* obs, const EventArg& e, T& t) {
			auto it = std::find(m_observerList.begin(), m_observerList.end(), obs);
			if (it != m_observerList.end()) {
				(*it)->update(this, e, t);
			}
		}

	private:
		std::vector<Observer<T>*> m_observerList;
	};
}
#endif
