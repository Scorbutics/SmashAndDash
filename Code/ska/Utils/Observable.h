#ifndef DEF_OBSERVABLE
#define DEF_OBSERVABLE

#include <string>
#include <vector>

class Input;
class Observer;
class EventArg;
namespace ska {
	class Observable
	{
	public:
		void addObserver(Observer* obs);
		void removeObserver(Observer* obs);
		void notifyObservers(EventArg* e);
		void notifyObserver(Observer* obs, EventArg* e);

	private:
		std::vector<Observer*> m_observerList;
	};
}
#endif
