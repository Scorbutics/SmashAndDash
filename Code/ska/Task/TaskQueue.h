#pragma once
#include <queue>
#include <memory>

namespace ska {
	class Task;
	typedef std::unique_ptr<Task> TaskPtr;

	class TaskQueue
	{
	public:
		TaskQueue();
		~TaskQueue();

		void queueTask(TaskPtr& t);
		void refresh();
	private:
		std::queue<TaskPtr> m_tasks;
		TaskPtr m_current;
	};
}
