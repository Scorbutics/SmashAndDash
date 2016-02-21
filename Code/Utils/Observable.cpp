#include "Observable.h"
#include "Observer.h"
using namespace std;

void Observable::addObserver(Observer* obs) {
	vector<Observer*>::iterator temp = find(m_observerList.begin(), m_observerList.end(), obs);
	if (temp == m_observerList.end()) {
		m_observerList.push_back(obs);
	}
}

void Observable::removeObserver(Observer* obs) {
	vector<Observer*>::iterator temp = find(m_observerList.begin(), m_observerList.end(), obs);
	if (temp != m_observerList.end()) {
		m_observerList.erase(temp);
	}
}

void Observable::notifyObservers(EventArg* e) {
	for (auto &it : m_observerList) {
		it->update(this, e);
	}
}

void Observable::notifyObserver(Observer* obs, EventArg * e) {
	auto it = find(m_observerList.begin(), m_observerList.end(), obs);
	if (it != m_observerList.end()) {
		(*it)->update(this, e);
	}
}