#pragma once

#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>

class ThreadPool {
	const unsigned int numberOfThreads;
	bool shouldTerminate;
	std::mutex queueMutex;
	std::condition_variable mutexCondition;
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;

	auto threadLoop() -> void;

public:
	ThreadPool(const unsigned int);
	auto queueTask(const std::function<void()>&) -> void;
	auto unassingedTasks() -> std::queue<std::function<void()>>::size_type;
	auto busy() -> bool;
	~ThreadPool();
};

ThreadPool::ThreadPool(const unsigned int numbOfThreads = std::thread::hardware_concurrency()) :
	numberOfThreads(numbOfThreads != 0 ? numbOfThreads : 4),
	shouldTerminate(false),
	threads(std::vector<std::thread>()),
	tasks(std::queue<std::function<void()>>())
{
	this->threads.reserve(this->numberOfThreads);
	for (unsigned int i = 0; i < this->numberOfThreads; i++)
		this->threads.push_back(
			std::thread(
				[this]() { this->threadLoop(); }
			)
		);
}

auto ThreadPool::threadLoop() -> void {
	while (true) {
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(this->queueMutex);
			this->mutexCondition.wait(lock, [this] {
				return !this->tasks.empty() || this->shouldTerminate;
			});
			if (this->shouldTerminate) return;
			task = this->tasks.front();
			this->tasks.pop();
		}
		task();
	}
}
auto ThreadPool::queueTask(const std::function<void()>& task) -> void {
	{
		std::unique_lock<std::mutex> lock(this->queueMutex);
		this->tasks.push(task);
	}
	this->mutexCondition.notify_one();
}
auto ThreadPool::unassingedTasks() -> std::queue<std::function<void()>>::size_type {
	std::queue<std::function<void()>>::size_type len;
	{
		std::unique_lock<std::mutex> lock(this->queueMutex);
		len = this->tasks.size();
	}
	return len;
}
auto ThreadPool::busy() -> bool {
	bool poolBusy;
	{
		std::unique_lock<std::mutex> lock(this->queueMutex);
		poolBusy = !this->tasks.empty();
	}
	return poolBusy;
}

ThreadPool::~ThreadPool() {
	{
		std::unique_lock<std::mutex> lock(this->queueMutex);
		this->shouldTerminate = true;
	}
	this->mutexCondition.notify_all();
	for (std::thread& activeThread : this->threads)
		activeThread.join();
}
