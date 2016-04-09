#pragma once
#include <functional>
#include <utility>
#include <vector>
#include <functional>
#include "../../Utils/Identity.h"

namespace ska {
	template<int ...>
	struct UnpackSeq { };

	template<int N, int ...S>
	struct Gens : Gens<N - 1, N - 1, S...> { };

	template<int ...S>
	struct Gens<0, S...> {
		typedef UnpackSeq<S...> type;
	};


	template <typename ...T>
	class TaskReceiver : public std::tuple<T...> {
	};

	template <typename ...T>
	class TaskSender : public std::tuple<T...> {
	};

	template <typename ...A>
	class ITask {
		template <typename RR, typename RA, typename SA> friend class Task;
	protected:
		std::tuple<A...> m_forwardArgs;
	};

	template <typename RR, typename RA, typename SA>
	class Task;

	template <typename RR, typename ...RA, typename ...SA>
	class Task<RR, TaskReceiver<RA...>, TaskSender<SA...>> : public ITask<SA...>{
	public:

		Task(typename Identity<std::function<RR(Task<RR, TaskReceiver<RA...>, TaskSender<SA...>>&, RA...)>>::type const& f) :
			m_f(f) {
			m_previous = nullptr;
		}

		Task(typename Identity<std::function<RR(Task<RR, TaskReceiver<RA...>, TaskSender<SA...>>&, RA...)>>::type const& f, ITask<RA...>& previous) :
			Task(f) {
			m_previous = &previous;
		}

		template <int ...N>
		RR run(UnpackSeq<N...> seq) {
			return (m_previous != nullptr) ?
				m_f(*this, std::get<N>(m_previous->m_forwardArgs)...) :
				m_f(*this, std::get<N>(m_empty)...);
		}

		RR operator()() {
			return run(typename Gens<sizeof...(RA)>::type());
		}

		void forward(SA ...args) {
			ITask<SA...>::m_forwardArgs = std::make_tuple(std::forward<SA>(args)...);
		}

		virtual ~Task() {
		}

	private:
		std::function<RR(Task<RR, TaskReceiver<RA...>, TaskSender<SA...>>&, RA...)> m_f;
		ITask<RA...>* m_previous;
		std::tuple<RA...> m_empty;
	};

}