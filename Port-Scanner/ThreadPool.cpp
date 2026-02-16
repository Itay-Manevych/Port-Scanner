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

				tasks_condition_variable.wait(lock, [this] { // avoid busy waiting!
					return stop || !tasks.empty(); // wake condition
				});

				if (stop && tasks.empty()) {
					return;
				}

				task = std::move(tasks.front());
				tasks.pop();

				active_tasks_amount++;

				lock.unlock(); // unlock BEFORE running the task

				task();

				lock.lock();
				active_tasks_amount--;

				if (active_tasks_amount == 0 && tasks.empty()) {
					active_condition_variable.notify_all();
				}
				lock.unlock();
			}
		});
	}
}

ThreadPool::~ThreadPool()
{
	std::unique_lock<std::mutex> lock(tasks_mutex);
	stop = true;
	lock.unlock();

	tasks_condition_variable.notify_all();
	active_condition_variable.notify_all();

	for (std::thread& t : worker_threads) {
		t.join();
	}
}

void ThreadPool::Enqueue(std::function<void()> task)
{
	std::unique_lock<std::mutex> lock(tasks_mutex);
	
	if (stop) {
		return;
	}

	tasks.push(std::move(task));
	
	lock.unlock();
	tasks_condition_variable.notify_one();
}

void ThreadPool::WaitIdle()
{
	std::unique_lock<std::mutex> lock(tasks_mutex);
	active_condition_variable.wait(lock, [this] {
		return active_tasks_amount == 0 && tasks.empty();
	});
}
