#pragma once
#include <functional>
#include "../Exceptions/IllegalArgumentException.h"


namespace ska {
    template <typename T, typename ...Args>
	class Observable;


	template <class T, class ...Args>
	class Observer {
    public:
        template <typename T1, typename ...Args1>
		friend class Observable;

		explicit Observer(std::function<bool(T&, Args...)> const& handler) : m_handler(handler) {
			if(m_handler == nullptr) {
				throw IllegalArgumentException("Function cannot be null", "Error");
			}
		}

		virtual ~Observer() = default;

	private:
		std::function<bool(T&, Args...)> m_handler;

		bool receive(T& evt, Args&... args) {
			return (m_handler)(evt, std::forward<Args&>(args)...);
		}

	};
}
