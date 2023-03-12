/*
 * @Author: Yaxin
 * @Date: 2023-03-12 20:18:36
 * @LastEditors: Yaxin
 * @LastEditTime: 2023-03-12 21:08:19
 */
#pragma once

#include <cstdio>

#include "singleton.h"

namespace designpat::singleton::test {

// SingletonWithDefaultConstructor, 继承自Singleton，演示使用无参构造函数的单例类实现
class SingletonWithDefaultConstructor
    : public Singleton<SingletonWithDefaultConstructor> {
 public:
  void foo() const { printf("foo called, a=%d\n", a_); }

 private:
  friend Singleton<SingletonWithDefaultConstructor>;
  SingletonWithDefaultConstructor() {
    printf("SingletonWithDefaultConstructor called\n");
  }

  int a_ = 0;
};

// SingletonWithCustomConstructor, 继承自SingletonWithArgs，演示具有多种构造函数的单例类实现
class SingletonWithCustomConstructor
    : public SingletonWithArgs<SingletonWithCustomConstructor> {
 public:
  void foo() const { printf("foo called, a=%d, b=%d, c=%d\n", a_, b_, c_); }

 private:
  friend SingletonWithArgs<SingletonWithCustomConstructor>;
  SingletonWithCustomConstructor() {
    printf("SingletonWithCustomConstructor called\n");
  }
  explicit SingletonWithCustomConstructor(int a) : a_(a) {
    printf("SingletonWithCustomConstructor(int a) called\n");
  }
  SingletonWithCustomConstructor(int a, int b) : a_(a), b_(b) {
    printf("SingletonWithCustomConstructor(int a, int b) called\n");
  }
  SingletonWithCustomConstructor(int a, int b, int c) : a_(a), b_(b), c_(c) {
    printf("SingletonWithCustomConstructor(int a, int b, int c) called\n");
  }

  int a_ = 0;
  int b_ = 0;
  int c_ = 0;
};

}  // namespace designpat::singleton::test
