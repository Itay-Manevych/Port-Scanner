#pragma once
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <vector>
#include <queue>
#include <thread>

class ThreadPool {
	public:
		ThreadPool(size_t num_threads = std::thread::hardware_concurrency());

		~ThreadPool();

		void Enqueue(std::function<void()> task);

		void WaitIdle();

	private:
		std::vector<std::thread> worker_threads; 
		
		std::queue<std::function<void()>> tasks;

		std::mutex tasks_mutex;

		std::condition_variable tasks_condition_variable; // in order to wake up sleeping therads - stops busy waiting

		bool stop = false;

		size_t active_tasks_amount = 0;

		std::condition_variable active_condition_variable;
};