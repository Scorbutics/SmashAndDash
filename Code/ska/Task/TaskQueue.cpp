#include <algorithm>
#include "TaskQueue.h"
#include "Task.h"
#include "Runnable.h"

ska::TaskQueue::TaskQueue() : m_current(nullptr), m_previous(nullptr) {
}

void ska::TaskQueue::queueTask(RunnablePtr& t) {
	m_tasks.push(std::move(t));
}

bool ska::TaskQueue::hasRunningTask() const {
	return !m_tasks.empty();
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
		m_previous = std::move(m_current);
		m_current = nullptr;
	}
}


ska::TaskQueue::~TaskQueue()
{
}
