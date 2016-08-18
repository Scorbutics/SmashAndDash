#pragma once
#include <string>
#include <functional>

namespace ska {

	template <class T, class ...Args>
	class Observer {
		
		template <class T1, class ...Args1>
		friend class Observable;

	public:
		Observer(std::function<void(const T&, Args...)> const& handler) : m_handler(handler), 
			m_right(nullptr), m_left(nullptr) {
		}

		virtual ~Observer() = default;

	private:
		std::function<void(const T&, Args...)> m_handler;
		Observer<T, Args...> * m_right;
		Observer<T, Args...> * m_left;

		void receive(const T& evt, Args&... args) {
			(m_handler)(evt, std::forward<Args&>(args)...);
		}

	};
}
