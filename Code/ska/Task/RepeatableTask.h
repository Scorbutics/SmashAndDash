#pragma once
#include <memory>
#include "Runnable.h"
#include "Task.h"

namespace ska {
	template <typename RA, typename SA>
	class RepeatableTask;

	template <typename ...RA, typename ...SA>
	class RepeatableTask<TaskReceiver<RA...>, TaskSender<SA...>> : public Task<bool, TaskReceiver<RA...>, TaskSender<SA...>>, public Runnable{
	public:
		/* Specilization only when the task doesn't receive any data from a previous one. The previous task shall NOT be provided */
		RepeatableTask(typename meta::Identity<std::function<bool(Task<bool, TaskReceiver<>, TaskSender<SA...>>&, RA...)>>::type const& f) :
			Task<bool, TaskReceiver<>, TaskSender<SA...>>(f) {
			}

		/* Global implementation when the task needs to receive data from a previous data. The previous task shall be provided */
		RepeatableTask(typename meta::Identity<std::function<bool(Task<bool, TaskReceiver<RA...>, TaskSender<SA...>>&, RA...)>>::type const& f, ITask<RA...>& previous) :
			Task<bool, TaskReceiver<RA...>, TaskSender<SA...>>(f, previous) {
			}

		virtual ~RepeatableTask() {
		}

		virtual bool operator()() override {
			return Task<bool, TaskReceiver<RA...>, TaskSender<SA...>>::operator()();
		}
	};

	template <typename RA, typename SA>
	using RepeatableTaskPtr = std::unique_ptr<RepeatableTask<TaskReceiver<RA>, TaskSender<SA>>>;
}
