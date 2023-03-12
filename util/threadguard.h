/*
 * @Author: Yaxin
 * @Date: 2023-03-12 20:18:36
 * @LastEditors: Yaxin
 * @LastEditTime: 2023-03-12 21:08:02
 */
#pragma once

#include <thread>

namespace designpat::util {

// ThreadGuard, 线程封装，析构时等待线程结束
class ThreadGuard {
 public:
  explicit ThreadGuard(std::thread thread) : thread_(std::move(thread)) {}
  ~ThreadGuard() {
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  //只可移动，不可复制
  ThreadGuard(ThreadGuard&&) = default;
  ThreadGuard& operator=(ThreadGuard&&) = default;
  ThreadGuard(const ThreadGuard&) = delete;
  ThreadGuard& operator=(const ThreadGuard&) = delete;

 private:
  std::thread thread_;
};

}  // namespace designpat::util