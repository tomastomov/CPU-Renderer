#pragma once
#include <thread>
#include <future>

namespace CPURenderer {
	struct ThreadPoolJob {
		std::move_only_function<void()> fn;
		std::promise<void> promise;
	};

	class ThreadPool {
	private:
		uint8_t _threadCount;
		std::vector<std::thread> _workers;
		std::vector<ThreadPoolJob> _tasks;
		std::mutex _mutex;
		std::condition_variable _cv;
		bool _stopping;

		void WorkerLoop();
	public:
		ThreadPool(uint8_t threadCount);
		~ThreadPool();

		std::future<void> SubmitJob(std::move_only_function<void()> fn);
	};
}