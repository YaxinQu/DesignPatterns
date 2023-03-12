/*
 * @Author: Yaxin
 * @Date: 2023-03-12 20:18:36
 * @LastEditors: Yaxin
 * @LastEditTime: 2023-03-12 21:07:37
 */
#pragma once

namespace designpat::util {

class NonCopyable {
 protected:
  NonCopyable() = default;
  ~NonCopyable() = default;

  NonCopyable(const NonCopyable& other) = delete;
  NonCopyable& operator=(const NonCopyable& other) = delete;
};

}  // namespace designpat::util