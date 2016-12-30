#ifndef BABYLON_CORE_SHARED_QUEUE_H
#define BABYLON_CORE_SHARED_QUEUE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Multiple producer, multiple consumer thread safe queue
 * Since 'return by reference' is used this queue won't throw
 *
 * Ref: "C++ Concurrency In Action" http://www.manning.com/williams
 */
template <typename T>
class BABYLON_SHARED_EXPORT SharedQueue {

public:
  SharedQueue& operator=(const SharedQueue&) = delete;
  SharedQueue(const SharedQueue& other)      = delete;

  SharedQueue()
  {
  }

  void push(T item)
  {
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _queue.push(std::move(item));
    }
    _data_cond.notify_one();
  }

  // return immediately, with true if successful retrieval
  bool tryAndPop(T& poppedItem)
  {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_queue.empty()) {
      return false;
    }
    poppedItem = std::move(_queue.front());
    _queue.pop();
    return true;
  }

  // Try to retrieve, if no items, wait till an item is available and try again
  void waitAndPop(T& poppedItem)
  {
    std::unique_lock<std::mutex> lock(_mutex);
    while (_queue.empty()) {
      _data_cond.wait(lock);
      //  This 'while' loop is equal to
      //  _data_cond.wait(lock, [](bool result){return !_queue.empty();});
    }
    poppedItem = std::move(_queue.front());
    _queue.pop();
  }

  bool empty() const
  {
    std::lock_guard<std::mutex> lock(_mutex);
    return _queue.empty();
  }

  std::size_t size() const
  {
    std::lock_guard<std::mutex> lock(_mutex);
    return _queue.size();
  }

private:
  std::queue<T> _queue;
  mutable std::mutex _mutex;
  std::condition_variable _data_cond;

}; // end of class SharedQueue

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_SHARED_QUEUE_H
