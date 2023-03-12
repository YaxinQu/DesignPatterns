/*
 * @Author: Yaxin
 * @Date: 2023-03-12 20:18:36
 * @LastEditors: Yaxin
 * @LastEditTime: 2023-03-12 21:08:13
 */
#include "singleton_test.h"

#include <cstdio>
#include <thread>
#include <vector>

#include "../util/threadguard.h"

using designpat::util::ThreadGuard;

namespace designpat::singleton::test {

void TestSingletonWithDefaultConstructor() {
  // SingletonTest a; // error，无法通过其他方式创建单例对象
  SingletonWithDefaultConstructor::GetInstance().foo();
}

template <typename... Args>
void TestSingletonWithCustomConstructor(Args&&... args) {
  SingletonWithCustomConstructor::CreateInstance(std::forward<Args>(args)...);
  SingletonWithCustomConstructor::GetInstance()->foo();
}

template <typename Callable, typename... Args>
void TestInThreads(int thread_num, Callable&& fn, Args&&... args) {
  if (thread_num <= 0) {
    return;
  }

  std::vector<ThreadGuard> vec;
  vec.reserve(thread_num);
  for (int i = 0; i < thread_num; i++) {
    vec.emplace_back(
        std::thread(std::forward<Callable>(fn), std::forward<Args>(args)...));
  }
}

void TestSingleton() {
  TestInThreads(5, TestSingletonWithCustomConstructor<>);
  TestInThreads(5, TestSingletonWithCustomConstructor<int>, 1);
  TestInThreads(5, TestSingletonWithCustomConstructor<int, int>, 1, 2);
  TestInThreads(5, TestSingletonWithCustomConstructor<int, int, int>, 1, 2, 3);
  TestInThreads(5, TestSingletonWithDefaultConstructor);
}

}  // namespace designpat::singleton::test

int main() {
  designpat::singleton::test::TestSingleton();
  return 0;
}
