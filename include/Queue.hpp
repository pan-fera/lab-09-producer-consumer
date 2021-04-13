//
// Copyright [2021] <pan_fera>
//

#ifndef INCLUDE_QUEUE_HPP_
#define INCLUDE_QUEUE_HPP_

#include <iostream>
#include <mutex>
#include <queue>
#include <string>


template <typename T>
class Queue {
 public:
  Queue():_counter(0){}
  void push(T&& obj) {
    std::lock_guard<std::mutex> lock(_mut);
    _queue.push(obj);
    ++_counter;
  }
  T front() {
    std::lock_guard<std::mutex> lock(_mut);
    T _tmp = _queue.front();
    _queue.pop();
    return _tmp;
  }
  void pop() {
    std::lock_guard<std::mutex> lock(_mut);
    --_counter;
  }
  bool empty() {
    std::lock_guard<std::mutex> lock(_mut);
    return _queue.empty();
  }
  int _counter;

 private:
  std::mutex _mut;
  std::queue<T> _queue;
};

#endif  // INCLUDE_QUEUE_HPP_
