/*
 * @Author: Yaxin
 * @Date: 2023-03-12 20:18:36
 * @LastEditors: Yaxin
 * @LastEditTime: 2023-03-15 21:23:21
 */
#pragma once

#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>

#include "../util/noncopyable.h"

namespace designpat::observer {

// Subject 主题模板类
// 模板参数为Notify函数的参数，支持多个
template <typename... NotifyArgs>
class Subject : public util::NonCopyable {
 public:
  /**
   * @description: 添加成员函数类型的观察者，在异步通知线程中调用对象的成员函数时，需要确定该对象是否已经被销毁（可能出现对象先销毁，后执行通知操作的情况）。
   * 因此专门添加了针对成员函数的模板约束，要求传递对象的shared_ptr，在内部获取该对象的weak_ptr，并通过weak_ptr::lock函数来安全地操作对象
   * @param {MemFunc} mem_func 成员函数指针，原型应该为 void(T::*)(Args..., NotifyArgs...)，
   * 在AddObserver时只传入Args，在Notify时会补上NotifyArgs。
   * @param {const std::shared_ptr<T>&} obj_this 对象共享指针
   * @param {Args&&...} args 剩余参数
   * @return {int} 分配给观察者的唯一id，可用于后续移除观察者
   */
  template <typename MemFunc, typename T, typename... Args>
  typename std::enable_if<std::is_member_function_pointer<MemFunc>::value, int>::type
  AddObserver(MemFunc mem_func, const std::shared_ptr<T>& obj_this, Args&&... args) {
    std::weak_ptr<T> weak_this(obj_this);
    // need c++20
    auto new_callable = [mem_func, weak_this, ... front_args = std::forward<Args>(args)](
                            NotifyArgs&&... notify_args) {
      auto p = weak_this.lock();
      if (p) {
        (p.get()->*mem_func)(front_args..., std::forward<NotifyArgs>(notify_args)...);
      }
    };

    std::unique_lock lock(mutex_);
    observers_.emplace(++cur_observer_id_, new_callable);
    return cur_observer_id_;
  }

  /**
   * @description: 添加非成员函数类型的观察者
   * @param {Callable&&} callable 可调用对象，例如普通函数指针、std::function、lambda,
   * 原型应该为：void(*)(Args..., NotifyArgs...)，在AddObserver时只传入Args，在Notify时会补上NotifyArgs。
   * @param {Args&&...} args 剩余参数
   * @return {int} 分配给观察者的唯一id，可用于后续移除观察者
   */
  template <typename Callable, typename... Args>
  typename std::enable_if<!std::is_member_function_pointer<Callable>::value, int>::type
  AddObserver(Callable&& callable, Args&&... args) {
    // need c++20
    auto new_callable = [func = std::forward<Callable>(callable),
                         ... front_args =
                             std::forward<Args>(args)](NotifyArgs&&... notify_args) {
      func(front_args..., std::forward<NotifyArgs>(notify_args)...);
    };

    std::unique_lock lock(mutex_);
    observers_.emplace(++cur_observer_id_, new_callable);
    return cur_observer_id_;
  }

  /**
   * @description: 移除观察者
   * @param {int} observer_id 观察者id，由AddObserver返回
   * @return {*}
   */
  void RemoveObserver(int observer_id) {
    std::unique_lock lock(mutex_);
    observers_.erase(observer_id);
  }

  /**
   * @description: 同步通知，逐个通知观察者，无法防止个别恶意观察者卡死整个通知流程
   * @param {NotifyArgs&&...} args 通知的参数列表
   * @return {*}
   */
  void Notify(NotifyArgs&&... args) {
    std::unique_lock lock(mutex_);
    for (auto iter : observers_) {
      iter.second(std::forward<NotifyArgs>(args)...);
    }
  }

  /**
   * @description: 异步通知，每个观察者新开线程进行通知，可防止个别恶意观察者卡死整个通知流程，但是新开线程会导致一定的资源消耗
   * @param {NotifyArgs&&...} args 通知的参数列表
   * @return {*}
   */
  void NotifyAsyn(NotifyArgs&&... args) {
    std::unique_lock lock(mutex_);
    for (auto iter : observers_) {
      auto t = std::thread(iter.second, std::forward<NotifyArgs>(args)...);
      t.detach();
    }
  }

 private:
  using Fn = std::function<void(NotifyArgs...)>;

  std::mutex mutex_;
  int cur_observer_id_ = 0;
  std::map<int, Fn> observers_;
};

}  // namespace designpat::observer
