#include "ThreadPool.h"

namespace CPURenderer {
	void ThreadPool::WorkerLoop()
	{
		while (true) {
			std::unique_lock lock(_mutex);

			_cv.wait(lock, [this] {
				return !_tasks.empty();
			});

			auto task = std::move(_tasks.back());
			_tasks.pop_back();

			lock.unlock();

			task.fn();
			task.promise.set_value();
		}
	}

	ThreadPool::ThreadPool(uint8_t threadCount)
	{
		_threadCount = threadCount;
		_workers.reserve(threadCount);

		for (int i = 0; i < threadCount; i++) {
			_workers.push_back(std::thread([this] { WorkerLoop(); }));
		}
	}

	ThreadPool::~ThreadPool() {

	}
	std::future<void> ThreadPool::SubmitJob(std::move_only_function<void()> fn)
	{
		std::promise<void> p;
		auto future = p.get_future();

		ThreadPoolJob job{
			std::move(fn),
			std::move(p)
		};

		{
			std::unique_lock lock(_mutex);

			_tasks.push_back(std::move(job));
		}

		_cv.notify_one();

		return future;
	};
}