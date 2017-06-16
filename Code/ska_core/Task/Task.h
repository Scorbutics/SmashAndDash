#pragma once
#include <functional>
#include "../Utils/Identity.h"
#include "../Utils/TupleUtils.h"

namespace ska {

	template <typename ...T>
	class TaskReceiver : public std::tuple<T...> {
	};

	template <typename ...T>
	class TaskSender : public std::tuple<T...> {
	};

	template <typename ...A>
	class ITask {
		template <typename RR, typename RA, typename SA> friend class Task;
		template <typename RR, typename RA, typename SA> friend class CompoundTask;

    public:
        ITask() : m_forwarded(false) {
        }

        void forward(A ...args) {
			m_forwardArgs = std::make_tuple(std::forward<A>(args)...);
			m_forwarded = true;
		}

	private:
		std::tuple<A...> m_forwardArgs;
		bool m_forwarded;
	};

	template <typename RR, typename RA, typename SA>
	class Task;

	template <typename RR, typename ...RA, typename ...SA>
	class Task<RR, TaskReceiver<RA...>, TaskSender<SA...>> : public ITask<SA...> {
	public:

        using F = std::function<RR(Task<RR, TaskReceiver<RA...>, TaskSender<SA...>>&, RA...)>;
        using LAMBDA = typename meta::Identity<F>::type;

		explicit Task(LAMBDA const& f) :
			m_f(f),
			m_previous(nullptr) {
		}

		Task(LAMBDA const& f, ITask<RA...>& previous) :
			m_f(f),
			m_previous(&previous) {
		}

        virtual RR operator()() {
			return run(meta::SeqRange<sizeof...(RA)>());
		}

		virtual ~Task() {
		}

        void forcePrevious(ITask<RA...>& previous) {
            m_previous = &previous;
        }

	private:
		template <int ...N>
		RR run(meta::SeqList<N...>) {
			return (m_previous != nullptr) ?
				m_f(*this, std::get<N>(m_previous->m_forwardArgs)...) :
				m_f(*this, std::get<N>(m_empty)...);
		}


		F m_f;
		ITask<RA...>* m_previous;
		std::tuple<RA...> m_empty;
	};

}
