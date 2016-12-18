#pragma once
#include <functional>
#include <unordered_map>

namespace ska {
	class Widget;

	template <class V>
	class UiBinder {
	public:
		UiBinder() {
		}

		template <class W>
		void bind(W& w, std::function<void(V)> func) {
			m_functions[&w] = func;
			w.addHandler<ValueChangedEventListener<V>>([&](Widget* tthis, ValueChangedEvent<V>& e) {	
				m_functions[tthis](e.getValue());
			});
		}

		~UiBinder() = default;
	
	private:
		std::unordered_map<Widget*, std::function<void(V)>> m_functions;

	};
}