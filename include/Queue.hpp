//
// Created by hacker on 4/11/21.
//

#ifndef TEMPLATE_QUEUE_HPP
#define TEMPLATE_QUEUE_HPP

#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <shared_mutex>

template <typename T>
class Queue {
 public:
  void push(T& obj) {
    std::lock_guard<std::shared_mutex> lock(_mut);
    _queue.push(obj);
  }
  T front() {
    _mut.lock_shared();
    T _tmp = _queue.front();
    _mut.unlock_shared();
    return _tmp;
  }
  void pop() {
    std::lock_guard<std::shared_mutex> lock(_mut);
    _queue.pop();
  }
  bool empty() {
    std::lock_guard<std::shared_mutex> lock(_mut);
    return _queue.empty();
  }

 private:
  std::shared_mutex _mut;
  std::queue<T> _queue;
};

#endif  // TEMPLATE_QUEUE_HPP
