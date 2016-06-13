#pragma once
#include <queue>
#include <memory>


namespace ska {
	class Runnable;
	typedef std::unique_ptr<Runnable> RunnablePtr;

	class TaskQueue
	{
	public:
		TaskQueue();
		~TaskQueue();

		bool hasRunningTask() const;
		void queueTask(RunnablePtr& t);
		void refresh();
	private:
		std::queue<RunnablePtr> m_tasks;
		RunnablePtr m_previous;
		RunnablePtr m_current;
	};
}
