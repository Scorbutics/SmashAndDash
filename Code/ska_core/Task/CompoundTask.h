#pragma once
#include <iostream>
#include "Task.h"
#include "../Exceptions/IllegalStateException.h"

namespace ska {
    template <typename RR, typename RA, typename SA>
	class CompoundTask;

    template <typename RR, typename ...RA, typename ...SA>
        class CompoundTask<RR, TaskReceiver<RA...>, TaskSender<SA...>>:
            public Task<bool, TaskReceiver<RA...>, TaskSender<SA...>>,
            public Runnable {

    using TBOOL = Task<bool, TaskReceiver<RA...>, TaskSender<SA...>>;
    using T = Task<RR, TaskReceiver<RA...>, TaskSender<SA...>>;
    using TPTR = std::unique_ptr<T>;
    using CT = CompoundTask<RR, TaskReceiver<RA...>, TaskSender<SA...>>;

    public:
        CompoundTask<RR, TaskReceiver<RA...>, TaskSender<SA...>>(TPTR&& task1, TPTR&& task2) :
        TBOOL(std::bind(&CT::runChildren, this)),
            m_task1(std::move(task1)),
            m_task2(std::move(task2)),
            m_task1Done(false),
            m_task2Done(false) {
        }

        CompoundTask<RR, TaskReceiver<RA...>, TaskSender<SA...>>(TPTR&& task1, TPTR&& task2, ska::ITask<RA...>& previous) :
        TBOOL(std::bind(&CT::runChildren, this), previous),
            m_task1(std::move(task1)),
            m_task2(std::move(task2)),
            m_task1Done(false),
            m_task2Done(false) {
                m_task1->forcePrevious(previous);
                m_task2->forcePrevious(previous);
        }

        virtual bool operator()() override {
			 return TBOOL::operator()();
		}

        virtual ~CompoundTask() {
        }

    private:
        bool runChildren() {
            if(!m_task1Done) {
                m_task1Done = !(*m_task1)();
            }

            if(!m_task2Done) {
                m_task2Done = !(*m_task2)();
            }

            if(m_task1Done && m_task2Done) {
                const auto& t1Forwarded = m_task1->m_forwarded;
                const auto& t2Forwarded = m_task2->m_forwarded;

                if(t1Forwarded && t2Forwarded && m_task1->m_forwarded != m_task2->m_forwarded) {
                    throw ska::IllegalStateException("2 compound tasks doesn't forward the same parameter");
                }

                if(!t2Forwarded && t1Forwarded) {
                    ITask<SA...>::m_forwardArgs = m_task1->m_forwardArgs;
                } else {
                    ITask<SA...>::m_forwardArgs = m_task2->m_forwardArgs;
                }
                return false;
            }
            return true;
        }

    private:
        TPTR m_task1;
        TPTR m_task2;
        bool m_task1Done;
        bool m_task2Done;
    };
}
