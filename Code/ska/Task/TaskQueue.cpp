#include <algorithm>
#include "TaskQueue.h"
#include "Task.h"

ska::TaskQueue::TaskQueue() : m_current(nullptr) {
}

void ska::TaskQueue::queueTask(TaskPtr& t) {
	m_tasks.push(std::move(t));
}

void ska::TaskQueue::refresh() {
	if (m_current == nullptr) {
		m_current = m_tasks.empty() ? nullptr : std::move(m_tasks.front());
		if (m_current == nullptr) {
			return;
		}
		m_tasks.pop();
	}

	if (!(*m_current)()) {
		m_current = nullptr;
	}
}


ska::TaskQueue::~TaskQueue()
{
}
