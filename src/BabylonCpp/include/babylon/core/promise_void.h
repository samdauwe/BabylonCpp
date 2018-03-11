#ifndef BABYLON_CORE_PROMISE_VOID_H
#define BABYLON_CORE_PROMISE_VOID_H

#include <atomic>
#include <functional>
#include <memory>

#include <babylon/core/promise.h>

namespace BABYLON {

template <>
class Promise<void> {
public:
  /**
   * @brief Default construction.
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
   * @param The Promise to be moved.
   */
  Promise(Promise&& other) : m_state(std::move(other.m_state))
  {
    other.m_state = nullptr;
  }

  /**
   * @brief Returns a future associated with this promise.
   * @return A future associated with this promise.
   */
  Future<void> future(void);

  /**
   * @brief Resolves the promise as a success.
   */
  void resolve(void)
  {
    m_state->resolved = true;
    if (m_state->resolve) {
      m_state->resolve();
    }
  }

  /**
   * @brief Rejects the promise as a failure.
   * @param The exception representing the error.
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
      throw ::std::runtime_error("Cannot reset an unfinished promise.");
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
   */
  Promise& operator=(const Promise&) = delete;

  /**
   * @brief Moves the promise from `other` into `this`.
   * @param other The Promise object to be moved.
   * @return * @return Reference to the Promise object.
   */
  Promise& operator=(Promise&& other)
  {
    m_state       = std::move(other.m_state);
    other.m_state = nullptr;
    return *this;
  }

private:
  template <typename Type>
  friend class Future;

  /**
   * @brief The container for the shared state between promises and futures.
   */
  struct _SharedState {
    std::atomic_bool resolved;
    std::atomic_bool rejected;
    std::function<void(void)> resolve;
    std::function<void(const std::exception&)> reject;
  };
  typedef std::shared_ptr<_SharedState> _SharedStatePtr;

  /**
   * @brief The state of the promise.
   */
  _SharedStatePtr m_state;

}; // end of class Promise<void>

template <>
class Future<void> {
public:
  /**
   * @brief The type promised by this future.
   */
  typedef void result_type;

  /**
   * @brief The type of Promise that made this future.
   */
  typedef Promise<void> promise_type;

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
  friend class Promise;

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
            = typename std::result_of<Resolve(Promise<Result>&&)>::type>
  Future<Result> _then(Resolve&& resolve, Reject&& reject);

  /**
   * @brief Chaining for generic functors promising nothing.
   * @tparam Resolve A functor type that can take a `Promise&&` as its
   * parameter.
   * @param resolve The functor to call when this one is resolved.
   * @return A new future, for when the provided `resolve` completes its action.
   */
  template <typename Resolve, typename Reject,
            typename = typename std::result_of<Resolve(Promise<>&&)>::type>
  Future<> _then(Resolve&& resolve, Reject&& reject);

  /**
   * @brief Chaining for `Future`-returning functors.
   * @tparam Resolve A functor which returns a `Future`.
   * @param resolve The functor to call when this one is ready.
   * @return A `Future` which will be resolved by `resolve`.
   */
  template <
    typename Resolve, typename Reject,
    typename ResolveResult = typename std::result_of<Resolve()>::type,
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
            typename ResolveResult = typename std::result_of<Resolve()>::type,
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
            typename ResolveResult = typename std::result_of<Resolve()>::type,
            typename std::enable_if<
              std::is_void<ResolveResult>::value>::type* = nullptr>
  Future<> _then(Resolve&& resolve, Reject&& reject);

  /**
   * @brief Our internal shared state.
   */
  typename promise_type::_SharedStatePtr m_state;

}; // end of class Future<void>

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_PROMISE_VOID_H
