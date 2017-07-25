#pragma once
#include <queue>
#include <memory>
#include "Runnable.h"

namespace ska {
	typedef std::unique_ptr<Runnable> RunnablePtr;

	class TaskQueue {
	public:
		TaskQueue();
		TaskQueue(const TaskQueue&) = delete;
		TaskQueue& operator=(const TaskQueue&) = delete;
		virtual ~TaskQueue() = default;

		bool hasRunningTask() const;

		template<typename T>
		ska::Runnable& queueTask(std::unique_ptr<T>&& t) {
            m_tasks.push(move(t));
            return *m_tasks.back();
		}

		void refresh();

	private:
		std::queue<RunnablePtr> m_tasks;
		RunnablePtr m_previous;
		RunnablePtr m_current;
	};
}
