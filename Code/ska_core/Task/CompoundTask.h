#pragma once
#include <iostream>
#include "Task.h"

namespace ska {

    class CompoundTask:
        public Task {

    using TPTR = std::unique_ptr<Task>;
    using CT = CompoundTask;

    public:
        CompoundTask(TPTR&& task1, TPTR&& task2) :
        Task(std::bind(&CT::runChildren, this)),
            m_task1(std::move(task1)),
            m_task2(std::move(task2)),
            m_task1Done(false),
            m_task2Done(false) {
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
                return false;
            }
            return true;
        }

        TPTR m_task1;
        TPTR m_task2;
        bool m_task1Done;
        bool m_task2Done;
    };
}
