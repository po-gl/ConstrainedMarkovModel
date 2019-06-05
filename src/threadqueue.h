#ifndef THREADQUEUE_H
#define THREADQUEUE_H
#include <thread>
#include <queue>

/**
 * @brief A thread safe queue that notifies worker threads when items are pushed into it
 * @author Porter Glines 5/29/19
 */
template <typename T>
class ThreadQueue {
public:
  ThreadQueue(std::mutex *mutex, std::condition_variable *cv);

  void push(T item);

  bool pop(T &elem);

  bool empty();

  int size();

private:
  std::queue<T> baseQueue;
  std::mutex *mutex;
  std::condition_variable *cv;
};

// Inline definitions to avoid template linking errors
#include "threadqueue.inl"

#endif