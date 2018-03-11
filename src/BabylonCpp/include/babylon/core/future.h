#ifndef BABYLON_CORE_FUTURE_H
#define BABYLON_CORE_FUTURE_H

#include <babylon/babylon_global.h>
#include <babylon/core/active.h>
#include <babylon/core/move_on_copy.h>

namespace BABYLON {

/**
 * Generic helper function to avoid repeating the steps for managing
 * asynchronous task job (by active object) that returns a future results
 * could of course be made even more generic if done more in the way of
 * std::async, ref: http://en.cppreference.com/w/cpp/thread/async
 */
template <typename Func, class BgWorker>
::std::future<typename ::std::result_of<Func()>::type>
spawn_task(Func func, BgWorker* worker)
{
  typedef typename ::std::result_of<Func()>::type result_type;
  typedef ::std::packaged_task<result_type()> task_type;

  if (nullptr == worker) {
    auto p = ::std::make_shared<::std::promise<result_type>>();
    ::std::future<result_type> future_result = p->get_future();
    p->set_exception(::std::make_exception_ptr(
      ::std::runtime_error("nullptr instantiated worker")));
    return future_result;
  }

  task_type task(::std::move(func));

  ::std::future<result_type> result = task.get_future();
  worker->send(MoveOnCopy<task_type>(::std::move(task)));
  return ::std::move(result);
}

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_FUTURE_H
