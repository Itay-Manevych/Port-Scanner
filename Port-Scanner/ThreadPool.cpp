#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads)
{
	if (num_threads == 0) {
		num_threads = 1;
	}

	for (size_t i = 0; i < num_threads; i++) {
		worker_threads.emplace_back([this] { // this in order to have ThreadPool members
			while (true) {
				std::function<void()> task;

				std::unique_lock<std::mutex> lock(tasks_mutex);

				condition_variable.wait(lock, [this] { // avoid busy waiting!
					return stop || !tasks.empty(); // wake condition
				});

				if (stop && tasks.empty()) {
					return;
				}

				task = std::move(tasks.front());
				tasks.pop();

				lock.unlock(); // unlock BEFORE running the task

				task();
			}
		});
	}
}

ThreadPool::~ThreadPool()
{
	std::unique_lock<std::mutex> lock(tasks_mutex);
	stop = true;
	lock.unlock();

	condition_variable.notify_all();

	for (std::thread& t : worker_threads) {
		t.join();
	}
}

void ThreadPool::Enqueue(std::function<void()> task)
{
	std::unique_lock<std::mutex> lock(tasks_mutex);
	tasks.push(std::move(task));
	lock.unlock();
	condition_variable.notify_one();
}
