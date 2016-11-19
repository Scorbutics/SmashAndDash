#pragma once
#include <string>
#include <functional>


namespace ska {
    template <typename T, typename ...Args>
	class Observable;


	template <class T, class ...Args>
	class Observer {
    public:
        template <typename T1, typename ...Args1>
		friend class Observable;

		Observer(std::function<bool(T&, Args...)> const& handler) : m_handler(handler),
			m_right(nullptr), m_left(nullptr) {
		}

		virtual ~Observer() = default;

	private:
		std::function<bool(T&, Args...)> m_handler;
		Observer<T, Args...> * m_right;
		Observer<T, Args...> * m_left;

		bool receive(T& evt, Args&... args) {
			return (m_handler)(evt, std::forward<Args&>(args)...);
		}

	};
}
