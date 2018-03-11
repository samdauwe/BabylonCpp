#ifndef BABYLON_CORE_PROMISE_H
#define BABYLON_CORE_PROMISE_H

#include <atomic>
#include <functional>
#include <memory>
#include <type_traits>

/**
 * @brief Implementation of A+ style promises in C++
 * @see Promises/A+: https://promisesaplus.com/
 * @ref
 * https://stackoverflow.com/questions/11389338/deferred-promise-pattern-in-c
 * https://github.com/LifeWanted/liblw/blob/master/source/lw/event/Promise.hpp
 */
namespace BABYLON {

template <typename T>
class Future;

/**
 * @brief Determines if the given variable is a `Future`, or derives publicly
 * from `Future`.
 * @tparam T The type to check.
 */
template <typename T>
struct IsFuture : public std::integral_constant<bool, false> {
};

template <typename T>
struct IsFuture<Future<T>> : public std::integral_constant<bool, true> {
};

template <template <typename> class T, typename Value>
struct IsFuture<T<Value>>
    : public std::integral_constant<
        bool, std::is_base_of<Future<Value>, T<Value>>::value> {
};

/**
 * @brief Breaks down a type to the highest non-future layer.
 * @tparam T The type to break down.
 */
template <typename T>
struct UnwrapFuture {
  typedef T result_type;  ///< The type that will be promised.
  typedef Future<T> type; ///< The type of a future for this type.
};

template <typename T>
struct UnwrapFuture<Future<T>> : public UnwrapFuture<T> {
};

/**
 * @brief Determines the result of a function as a future.
 */
template <typename T>
struct FutureResultOf;

template <typename F, typename... Args>
struct FutureResultOf<F(Args...)>
    : public UnwrapFuture<typename std::result_of<F(Args...)>::type> {
};

/**
 * @brief `Promise`s and `Future`s allow for chaining callbacks without nesting.
 * A `Promise` is the active side of the pair. Asynchronous functions create a
 * promise and later fulfill it by either resolving or rejecting the promise.
 */
template <typename T = void>
class Promise {
public:
  /**
   * @brief  Default construction.
   */
  Promise(void) : m_state(new _SharedState())
  {
    m_state->resolved = false;
    m_state->rejected = false;
    m_state->resolve  = nullptr;
    m_state->reject   = nullptr;
  }

  /**
   * @brief No copying!
   */
  Promise(const Promise&) = delete;

  /**
   * @brief Moves the promise from `other` to `this`.
   * @param other The promise to move.
   */
  Promise(Promise&& other) : m_state(std::move(other.m_state))
  {
    other.m_state = nullptr;
  }
  /**
   * @brief Returns a future associated with this promise.
   * @return A future associated with this promise.
   */
  Future<T> future(void);

  /**
   * @brief Resolves the promise as a success.
   * @param value the value to resolve.
   */
  void resolve(T&& value)
  {
    m_state->resolved = true;
    if (m_state->resolve) {
      m_state->resolve(std::move(value));
    }
  }

  /**
   * @brief Resolves the promise as a success.
   * @param value the value to resolve.
   */
  void resolve(const T& value)
  {
    resolve(T(value));
  }

  /**
   * @brief Rejects the promise as a failure.
   * @param err The error to throw.
   */
  void reject(const std::exception& err)
  {
    m_state->rejected = true;
    if (m_state->reject) {
      m_state->reject(err);
    }
    else {
      throw err;
    }
  }

  /**
   * @brief Resets the promise's internal state so that it can be reused.
   * @throws PromiseError If the promise is in an unfinished state.
   */
  void reset(void)
  {
    if (!is_finished()) {
      throw std::runtime_error("Cannot reset an unfinished promise.");
    }
    m_state->resolved = false;
    m_state->rejected = false;
    m_state->resolve  = nullptr;
    m_state->reject   = nullptr;
  }

  /**
   * @brief Indicates if the promise has been resolved.
   * @return If the promise has been resolved.
   */
  bool is_resolved(void) const
  {
    return m_state->resolved;
  }

  /**
   * @brief Indicates if the promise has been rejected.
   * @return If the promise has been rejected.
   */
  bool is_rejected(void) const
  {
    return m_state->rejected;
  }

  /**
   * @brief Indicates if the promise has been either resolved or rejected.
   * @return If the promise has been either resolved or rejected.
   */
  bool is_finished(void) const
  {
    return is_resolved() || is_rejected();
  }

  /**
   * @brief No copying!
   * @return Reference to the Promise object.
   */
  Promise& operator=(const Promise&) = delete;

  /**
   * @brief Moves the promise from `other` into `this`.
   * @param other The Promise that should be moved.
   * @return Reference to the Promise object.
   */
  Promise& operator=(Promise&& other)
  {
    m_state       = std::move(other.m_state);
    other.m_state = nullptr;
    return *this;
  }

private:
  template <typename Type>
  friend class BABYLON::Future;

  /**
   * @brief The container for the shared state between promises and futures.
   */
  struct _SharedState {
    std::atomic_bool resolved;
    std::atomic_bool rejected;
    std::function<void(T&&)> resolve;
    std::function<void(const std::exception&)> reject;
  };
  typedef std::shared_ptr<_SharedState> _SharedStatePtr;

  /**
   * @brief The state of the promise.
   */
  _SharedStatePtr m_state;

}; // end of class Promise<T>

/**
 * @brief The passive half of the `Promise`-`Future` pair.
 * `Future`s are the requester's handle on an asynchronous event. They allow
 * callbacks to be registered for after it has been started.
 */
template <typename T = void>
class Future {
public:
  /**
   * @brief The type promised by this future.
   */
  typedef T result_type;

  /**
   * @brief The type of Promise that made this future.
   */
  typedef Promise<T> promise_type;

  template <typename Func>
  auto then(Func&& func)
  {
    return then(std::forward<Func>(func), nullptr);
  }

  template <typename Result, typename Func>
  auto then(Func&& func)
  {
    return then<Result>(std::forward<Func>(func), nullptr);
  }

  template <typename Resolve, typename Reject>
  auto then(Resolve&& resolve, Reject&& reject)
  {
    return _then(std::forward<Resolve>(resolve), std::forward<Reject>(reject));
  }

  template <typename Result, typename Resolve, typename Reject>
  auto then(Resolve&& resolve, Reject&& reject)
  {
    return _then<Result>(std::forward<Resolve>(resolve),
                         std::forward<Reject>(reject));
  }

  /**
   * @brief Connects this promise to the one provided.
   * @param promise The promise to resolve/reject with this one.
   */
  void then(promise_type&& promise);

private:
  template <typename Type>
  friend class BABYLON::Promise;

  /**
   * @brief Only `Promise`s can construct us.
   * @param state The shared state to associate with.
   */
  Future(const typename promise_type::_SharedStatePtr& state) : m_state(state)
  {
  }

  /**
   * @brief Chaining for generic functors.
   * @tparam Result  The type given functor promises.
   * @tparam Resolve A functor type that can take a `Promise&&` as its
   * parameter.
   * @param resolve The functor to call when this one is resolved.
   * @return A new future, for when the provided `resolve` completes its action.
   */
  template <typename Result, typename Resolve, typename Reject,
            typename
            = typename std::result_of<Resolve(T&&, Promise<Result>&&)>::type>
  Future<Result> _then(Resolve&& resolve, Reject&& reject);

  /**
   * @brief Chaining for generic functors promising nothing.
   * @tparam Resolve A functor type that can take a `Promise&&` as its
   * parameter.
   * @param resolve The functor to call when this one is resolved.
   * @return A new future, for when the provided `resolve` completes its action.
   */
  template <typename Resolve, typename Reject,
            typename = typename std::result_of<Resolve(T&&, Promise<>&&)>::type>
  Future<> _then(Resolve&& resolve, Reject&& reject);

  /**
   * @brief Chaining for `Future`-returning functors.
   * @tparam Resolve A functor which returns a `Future`.
   * @param resolve The functor to call when this one is ready.
   * @return A `Future` which will be resolved by `resolve`.
   */
  template <
    typename Resolve, typename Reject,
    typename ResolveResult = typename std::result_of<Resolve(T&&)>::type,
    typename std::enable_if<IsFuture<ResolveResult>::value>::type* = nullptr>
  Future<typename ResolveResult::result_type> _then(Resolve&& resolve,
                                                    Reject&& reject);

  /**
   * @brief Chaining for value-returning functors (i.e. synchronous ones).
   * @tparam Resolve A synchronous functor type.
   * @param resolve A synchronous functor returning some value.
   * @return A `Future` which will be resolved with the return value from
   * `resolve`.
   */
  template <typename Resolve, typename Reject,
            typename ResolveResult
            = typename std::result_of<Resolve(T&&)>::type,
            typename std::enable_if<
              !IsFuture<ResolveResult>::value
              && !std::is_void<ResolveResult>::value>::type* = nullptr>
  Future<ResolveResult> _then(Resolve&& resolve, Reject&& reject);

  /**
   * @brief Chaining for void-returning synchronous functors.
   * @tparam Resolve A synchronous functor type.
   * @param resolve A synchronous functor with no return value.
   * @return A `Future` which will be resolved `resolve` runs.
   */
  template <typename Resolve, typename Reject,
            typename ResolveResult
            = typename std::result_of<Resolve(T&&)>::type,
            typename std::enable_if<
              std::is_void<ResolveResult>::value>::type* = nullptr>
  Future<> _then(Resolve&& resolve, Reject&& reject);

  /**
   * @brief Our internal shared state.
   */
  typename promise_type::_SharedStatePtr m_state;

}; // end of class Future<T>

} // end of namespace BABYLON

#include <babylon/core/promise_impl.h>

#endif // end of BABYLON_CORE_PROMISE_H
