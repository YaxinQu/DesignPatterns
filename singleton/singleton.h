/*
 * @Author: Yaxin
 * @Date: 2023-03-12 20:18:36
 * @LastEditors: Yaxin
 * @LastEditTime: 2023-03-12 21:08:32
 */
#pragma once

#include <memory>
#include <mutex>
#include <type_traits>

#include "../util/noncopyable.h"

namespace designpat::singleton {

// noncopyable和单例的区别：
// noncopyable指对象创建后不能被复制，但是可以创建多个对象
// 单例是指这个类只能创建一个对象，该唯一对象也不能被复制

// 简单的单例实现
// 优点：线程安全、实现简单、用法简单、概念清晰
// 缺点：继承Singleton的子类需要提供一个无参的构造函数，并且inst是创建在静态区，不能在运行中销毁这块内存
// 用法：class DrivedClass : public Singleton<DrivedClass>...
//      DrivedClass需要提供无参的构造函数并将所有构造函数都设置为私有，还需要将Singleton<DrivedClass>设置为友元
template <typename T>
class Singleton : public util::NonCopyable {
 public:
  static T& GetInstance() noexcept(std::is_nothrow_constructible<T>::value) {
    static T inst;
    return inst;
  }

 protected:
  Singleton() = default;
  ~Singleton() = default;
};

// 较复杂的单例实现，也是较差的模式
// 优点：线程安全、可支持T多种构造函数
// 缺点：用法较复杂，需要2个接口，生成inst，获取inst。
template <typename T>
class SingletonWithArgs : public util::NonCopyable {
 public:
  template <typename... Args>
  static void CreateInstance(Args&&... args) noexcept(
      std::is_nothrow_constructible<T, Args...>::value) {
    static std::once_flag inst_flag;
    std::call_once(inst_flag,
                   [&] { inst_.reset(new T(std::forward<Args>(args)...)); });
  }

  static T* GetInstance() {
    if (!inst_) {
      throw "Singleton instance has not created now!";
    }

    return inst_.get();
  }

  // 提供销毁接口会增加很多潜在风险：即使单例类内部处理了多线程问题，外部在获取inst后保留了inst指针，如果另外一个模块销毁了inst，那还是会有问题。
  //static void DestoryInstance() {
  //}

 protected:
  SingletonWithArgs() = default;
  ~SingletonWithArgs() = default;

 private:
  inline static std::unique_ptr<T> inst_{};  // need c++17
};

}  // namespace designpat::singleton