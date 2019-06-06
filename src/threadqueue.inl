// Inline definitions

#include <mutex>
#include <condition_variable>

template<class T>
ThreadQueue<T>::ThreadQueue(std::mutex *mutex, std::condition_variable *cv) : mutex(mutex), cv(cv) {}

template<typename T>
void ThreadQueue<T>::push(T item) {
  std::unique_lock<std::mutex> lock(*mutex);
  baseQueue.push(item);

  lock.unlock(); // unlock mutex before notifying threads
  cv->notify_all();
}

template<typename T>
bool ThreadQueue<T>::pop(T &elem){
  std::unique_lock<std::mutex> lock(*mutex);
  if (!baseQueue.empty()) {
    elem = baseQueue.front();
    baseQueue.pop();
    return true;
  } else {
    return false;
  }
}

template<class T>
bool ThreadQueue<T>::empty() {
  std::unique_lock<std::mutex> lock(*mutex);
  return baseQueue.empty();
}


template<class T>
int ThreadQueue<T>::size() {
  std::unique_lock<std::mutex> lock(*mutex);
  return baseQueue.size();
}
