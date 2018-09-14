#ifndef BABYLON_CORE_ACTIVE_H
#define BABYLON_CORE_ACTIVE_H

#include <functional>
#include <thread>

#include <babylon/babylon_api.h>
#include <babylon/core/shared_queue.h>

namespace BABYLON {

/**
 * Example of a Active Object, using C++11 std::thread mechanisms to make it
 * safe for thread communication.
 *
 * This was originally published at
 * http://sites.google.com/site/kjellhedstrom2/active-object-with-cpp0x
 * and inspired from Herb Sutter's C++11 Active Object
 * http://herbsutter.com/2010/07/12/effective-concurrency-prefer-using-active-objects-instead-of-naked-threads
 */
class BABYLON_SHARED_EXPORT Active {

public:
  using Callback = std::function<void()>;

public:
  virtual ~Active()
  {
    send([this] { _done = true; });
    _thread.join();
  }

  void send(Callback _msg)
  {
    _messageQueue.push(_msg);
  }

  /// Factory: safe construction of object before thread start
  static std::unique_ptr<Active> createActive()
  {
    std::unique_ptr<Active> aPtr(new Active());
    aPtr->_thread = std::thread(&Active::run, aPtr.get());
    return aPtr;
  }

private:
  // Construction ONLY through factory createActive();
  Active() : _done(false)
  {
  }
  Active(const Active&) = delete;
  Active& operator=(const Active&) = delete;

  void run()
  {
    while (!_done) {
      Callback func;
      _messageQueue.waitAndPop(func);
      func();
    }
  }

  SharedQueue<Callback> _messageQueue;
  std::thread _thread;
  bool _done;

}; // end of class Active

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_ACTIVE_H
