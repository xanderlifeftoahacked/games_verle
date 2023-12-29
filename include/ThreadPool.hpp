#pragma once
#include "Constants.hpp"
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace utils {

class ThreadPool {
public:
  void start() {
    int threadsCount =
        constants::numberOfThreadsX * constants::numberOfThreadsY;
    while (threadsCount--) {
      threads.emplace_back(std::thread(&ThreadPool::threadLoop, this));
    }
  }

  void queueJob(const std::function<void()> &job) {
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      jobs.push(job);
    }
    mutexCondition.notify_one();
  }

  void waitForCompletion() {
    while (jobs.size()) {
      std::this_thread::yield();
    }
  }

private:
  void threadLoop() {
    for (;;) {
      std::function<void()> job;
      {
        std::unique_lock<std::mutex> lock(queueMutex);
        mutexCondition.wait(
            lock, [this] { return !jobs.empty() || shouldTerminate; });
        if (shouldTerminate)
          return;

        job = jobs.front();
        jobs.pop();
      }
      job();
    }
  }

  bool shouldTerminate = false;
  std::mutex queueMutex;
  std::condition_variable mutexCondition;
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> jobs;
};
} // namespace utils
