/*
 * @Author: Yaxin
 * @Date: 2023-03-12 20:18:36
 * @LastEditors: Yaxin
 * @LastEditTime: 2023-03-15 21:16:30
 */
#include "subject.h"

#include <chrono>
#include <cstdio>
#include <string>
#include <thread>

namespace designpat::observer::test {

struct Event {
  int event_id;
  std::string evetn_msg;
};

Subject<const Event&> subject;

// 使用成员函数的观察者
class ClassObserver {
 public:
  explicit ClassObserver(std::string name) : name_(std::move(name)) {}

  void OnNotify(const Event& event) {
    printf("%s::OnNotify called, event id is %d, msg is %s\n", name_.c_str(),
           event.event_id, event.evetn_msg.c_str());
  }

 private:
  std::string name_;
};

// 普通函数观察者
void OnNotify(const Event& event) {
  printf("OnNotify called, event id is %d, msg is %s\n", event.event_id,
         event.evetn_msg.c_str());
}

void TestInSignleThread() {
  // 添加普通函数观察者
  int func_obs_id = subject.AddObserver(OnNotify);

  // 添加lambda表达式
  int lambda_obs_id = subject.AddObserver([](const Event& event) {
    printf("lambda called, event id is %d, msg is %s\n", event.event_id,
           event.evetn_msg.c_str());
  });

  //添加普通类对象观察者
  auto class_observer = std::make_shared<ClassObserver>("class_observer");
  int class_obs_id = subject.AddObserver(&ClassObserver::OnNotify, class_observer);
  int tmp_class_obs_id = 0;
  {
    //添加临时类对象观察者
    auto tmp_class_observer = std::make_shared<ClassObserver>("tmp_class_observer");
    tmp_class_obs_id = subject.AddObserver(&ClassObserver::OnNotify, tmp_class_observer);

    // 同步通知，此时所有已添加的观察者都可以接收到通知
    subject.Notify({1, "sync notify in temp"});

    // 异步通知，tmp_class_observer可能已被销毁，可能无法接受到通知
    subject.NotifyAsyn({2, "async notify in temp"});
  }

  // tmp_class_observer已被销毁，无法接受到通知
  subject.Notify({3, "sync notify"});
  subject.NotifyAsyn({3, "async notify"});

  // 移除部分观察者
  subject.RemoveObserver(func_obs_id);
  subject.RemoveObserver(lambda_obs_id);

  subject.Notify({4, "sync notify after remove"});
  subject.NotifyAsyn({4, "async notify after remove"});

  // 移除全部观察者
  subject.RemoveObserver(class_obs_id);
  subject.RemoveObserver(tmp_class_obs_id);
  // 再次通知
  subject.Notify({5, "sync notify after remove all"});
  subject.NotifyAsyn({5, "async notify after remove all"});
}

void NotifyThreadFunc(int notify_count) {
  for (int i = 0; i < notify_count; ++i) {
    subject.Notify({1, "notify"});
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}

void ObserverHandleThreadFunc() {
  while (true) {
    printf("add observer\n");
    int id = subject.AddObserver(OnNotify);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    subject.RemoveObserver(id);
    printf("observer removed\n");
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}

void TestInMultiThreads() {
  auto notify_thread = std::thread(NotifyThreadFunc, 100);
  auto observer_thread = std::thread(ObserverHandleThreadFunc);
  observer_thread.detach();

  if (notify_thread.joinable()) {
    notify_thread.join();
  }
}

}  // namespace designpat::observer::test

int main() {

  designpat::observer::test::TestInSignleThread();

  std::this_thread::sleep_for(std::chrono::seconds(1));

  designpat::observer::test::TestInMultiThreads();

  return 0;
}