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
    using CT = CompoundTask<RR, TaskReceiver<RA...>, TaskSender<SA...>>;

    public:
        CompoundTask<RR, TaskReceiver<RA...>, TaskSender<SA...>>(std::unique_ptr<T>&& task1, std::unique_ptr<T>&& task2) :
        TBOOL(std::bind(&CT::runChildren, this)),
            m_task1(std::move(task1)),
            m_task2(std::move(task2)),
            m_task1Done(false),
            m_task2Done(false) {

        }

        CompoundTask<RR, TaskReceiver<RA...>, TaskSender<SA...>>(std::unique_ptr<T>&& task1, std::unique_ptr<T>&& task2, ska::ITask<SA...>& previous) :
        TBOOL(std::bind(&CT::runChildren, this), previous),
            m_task1(std::move(task1), previous),
            m_task2(std::move(task2), previous),
            m_task1Done(false),
            m_task2Done(false) {

        }

        virtual bool operator()() override {
			 return TBOOL::operator()();
		}

        virtual ~CompoundTask() {
        }

    private:
        bool runChildren() {
            std::cout << "run compound" << std::endl;
            if(!m_task1Done) {
                std::cout << "tttt begin !" << std::endl;
                m_task1Done = !(*m_task1)();
                std::cout << "tttt !" << std::endl;
            }

            if(!m_task2Done) {
                std::cout << "tututu begin !" << std::endl;
                m_task2Done = !(*m_task2)();
                std::cout << "tututu !" << std::endl;
            }

            if(m_task1Done && m_task2Done) {
                std::cout << "finish !" << std::endl;
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
        std::unique_ptr<T> m_task1;
        std::unique_ptr<T> m_task2;
        bool m_task1Done;
        bool m_task2Done;
    };
}
