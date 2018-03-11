#ifndef BABYLON_CORE_PROMISE_IMPL_H
#define BABYLON_CORE_PROMISE_IMPL_H

#include <babylon/core/promise.h>
#include <babylon/core/promise_void.h>

namespace BABYLON {

template <typename T>
inline Future<T> Promise<T>::future(void)
{
  return Future<T>(m_state);
}

inline Future<void> Promise<void>::future(void)
{
  return Future<void>(m_state);
}

template <typename T>
template <typename Result, typename Resolve, typename Reject, typename>
Future<Result> Future<T>::_then(Resolve&& resolve, Reject&& reject)
{
  auto next        = std::make_shared<Promise<Result>>();
  auto prev        = m_state;
  m_state->resolve = [resolve, prev, next](T&& value) mutable {
    resolve(std::move(value), std::move(*next));
    prev->reject = nullptr;
    prev.reset();
  };

  typedef std::function<void(const std::exception&)> RejectHandler;
  RejectHandler rejectHandler;
  if (std::is_same<std::nullptr_t, Reject>::value) {
    rejectHandler = [next](const std::exception& err) { next->reject(err); };
  }
  else {
    rejectHandler = std::forward<Reject>(reject);
  }
  m_state->reject
    = [rejectHandler, prev, next](const std::exception& err) mutable {
        rejectHandler(err);
        prev->resolve = nullptr;
        prev.reset();
      };

  return next->future();
}

template <typename T>
template <typename Resolve, typename Reject, typename>
inline Future<> Future<T>::_then(Resolve&& resolve, Reject&& reject)
{
  return then<void>(std::forward<Resolve>(resolve),
                    std::forward<Reject>(reject));
}

template <typename T>
template <typename Resolve, typename Reject, typename ResolveResult,
          typename std::enable_if<IsFuture<ResolveResult>::value>::type*>
Future<typename ResolveResult::result_type> Future<T>::_then(Resolve&& resolve,
                                                             Reject&& reject)
{
  typedef typename ResolveResult::result_type Result;
  return then<Result>(
    [resolve](T&& value, Promise<Result>&& promise) mutable {
      resolve(std::move(value)).then(std::move(promise));
    },
    std::forward<Reject>(reject));
}

template <typename T>
template <
  typename Resolve, typename Reject, typename ResolveResult,
  typename std::enable_if<!IsFuture<ResolveResult>::value
                          && !std::is_void<ResolveResult>::value>::type*>
Future<ResolveResult> Future<T>::_then(Resolve&& resolve, Reject&& reject)
{
  return then<ResolveResult>(
    [resolve](T&& value, Promise<ResolveResult>&& promise) mutable {
      try {
        promise.resolve(resolve(std::move(value)));
      }
      catch (const std::exception& err) {
        promise.reject(err);
      }
    },
    std::forward<Reject>(reject));
}

template <typename T>
template <typename Resolve, typename Reject, typename ResolveResult,
          typename std::enable_if<std::is_void<ResolveResult>::value>::type*>
Future<> Future<T>::_then(Resolve&& resolve, Reject&& reject)
{
  return then(
    [resolve](T&& value, Promise<>&& promise) {
      try {
        resolve(std::move(value));
      }
      catch (const std::exception& err) {
        promise.reject(err);
        return;
      }
      promise.resolve();
    },
    std::forward<Reject>(reject));
}

template <typename T>
void Future<T>::then(promise_type&& promise)
{
  auto next        = std::make_shared<promise_type>(std::move(promise));
  auto prev        = m_state;
  m_state->resolve = [prev, next](T&& value) mutable {
    next->resolve(std::move(value));
    prev->reject = nullptr;
    prev.reset();
  };
  m_state->reject = [prev, next](const std::exception& err) mutable {
    next->reject(err);
    prev->resolve = nullptr;
    prev.reset();
  };
}

template <typename Result, typename Resolve, typename Reject, typename>
Future<Result> Future<void>::_then(Resolve&& resolve, Reject&& reject)
{
  auto next        = std::make_shared<Promise<Result>>();
  auto prev        = m_state;
  m_state->resolve = [resolve, prev, next]() mutable {
    resolve(std::move(*next));
    prev->reject = nullptr;
    prev.reset();
  };

  typedef std::function<void(const std::exception&)> RejectHandler;
  RejectHandler rejectHandler;
  if (std::is_same<std::nullptr_t, Reject>::value) {
    rejectHandler = [next](const std::exception& err) { next->reject(err); };
  }
  else {
    rejectHandler = std::forward<Reject>(reject);
  }
  m_state->reject
    = [rejectHandler, prev, next](const std::exception& err) mutable {
        rejectHandler(err);
        prev->resolve = nullptr;
        prev.reset();
      };

  return next->future();
}

template <typename Resolve, typename Reject, typename>
Future<> Future<void>::_then(Resolve&& resolve, Reject&& reject)
{
  return then<void>(std::forward<Resolve>(resolve),
                    std::forward<Reject>(reject));
}

template <typename Resolve, typename Reject, typename ResolveResult,
          typename std::enable_if<IsFuture<ResolveResult>::value>::type*>
Future<typename ResolveResult::result_type>
Future<void>::_then(Resolve&& resolve, Reject&& reject)
{
  typedef typename ResolveResult::result_type Result;
  return then<Result>(
    [resolve](Promise<Result>&& promise) {
      resolve().then(std::move(promise));
    },
    std::forward<Reject>(reject));
}

template <
  typename Resolve, typename Reject, typename ResolveResult,
  typename std::enable_if<!IsFuture<ResolveResult>::value
                          && !std::is_void<ResolveResult>::value>::type*>
Future<ResolveResult> Future<void>::_then(Resolve&& resolve, Reject&& reject)
{
  return then<ResolveResult>(
    [resolve](Promise<ResolveResult>&& promise) {
      try {
        promise.resolve(resolve());
      }
      catch (const std::exception& err) {
        promise.reject(err);
      }
    },
    std::forward<Reject>(reject));
}

template <typename Resolve, typename Reject, typename ResolveResult,
          typename std::enable_if<std::is_void<ResolveResult>::value>::type*>
Future<void> Future<void>::_then(Resolve&& resolve, Reject&& reject)
{
  return then<void>(
    [resolve](Promise<void>&& promise) {
      try {
        resolve();
      }
      catch (const std::exception& err) {
        promise.reject(err);
        return;
      }
      promise.resolve();
    },
    std::forward<Reject>(reject));
}

inline void Future<void>::then(promise_type&& promise)
{
  auto next        = std::make_shared<promise_type>(std::move(promise));
  auto prev        = m_state;
  m_state->resolve = [prev, next]() mutable {
    next->resolve();
    prev->reject = nullptr;
    prev.reset();
  };
  m_state->reject = [prev, next](const std::exception& err) mutable {
    next->reject(err);
    prev->resolve = nullptr;
    prev.reset();
  };
}

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_PROMISE_IMPL_H
