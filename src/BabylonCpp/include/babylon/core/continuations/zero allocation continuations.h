#ifndef BABYLON_CORE_CONTINUATIONS_ZERO_ALLOCATION_CONTINUATIONS_TIMER_H
#define BABYLON_CORE_CONTINUATIONS_ZERO_ALLOCATION_CONTINUATIONS_TIMER_H

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <type_traits>
#include <utility>

/**
 * @brief Zero allocation continuations implementation.
 * @see https://vittorioromeo.info/index/blog/zeroalloc_continuations_p0.html
 * @see https://vittorioromeo.info/index/blog/zeroalloc_continuations_p1.html
 * @see https://vittorioromeo.info/index/blog/zeroalloc_continuations_p2.html
 * @see
 * https://github.com/SuperV1234/vittorioromeo.info/tree/master/extra/zeroalloc_continuations
 */

namespace BABYLON {

#define FWD(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

struct nothing {
};

template <typename T>
using void_to_nothing_t = std::conditional_t<std::is_void_v<T>, nothing, T>;

template <typename T>
using is_nothing_t = std::is_same<std::decay_t<T>, nothing>;

template <typename T>
constexpr bool is_nothing_v = is_nothing_t<T>::value;

template <typename F, typename... Ts>
decltype(auto) returning_nothing_instead_of_void(F&& f, Ts&&... xs)
{
  if constexpr (std::is_void_v<decltype(FWD(f)(FWD(xs)...))>) {
    FWD(f)(FWD(xs)...);
    return nothing{};
  }
  else {
    return FWD(f)(FWD(xs)...);
  }
}

template <typename F>
decltype(auto) call_ignoring_nothing(F&& f)
{
  return returning_nothing_instead_of_void(FWD(f));
}

template <typename F, typename T, typename... Ts>
decltype(auto) call_ignoring_nothing(F&& f, T&& x, Ts&&... xs)
{
  return call_ignoring_nothing(
    [&](auto&&... ys) -> decltype(auto) {
      if constexpr (is_nothing_v<T>) {
        return FWD(f)(FWD(ys)...);
      }
      else {
        return FWD(f)(FWD(x), FWD(ys)...);
      }
    },
    FWD(xs)...);
}

template <typename F, typename T>
decltype(auto) apply_ignoring_nothing(F&& f, T&& t)
{
  return std::apply(
    [&](auto&&... xs) -> decltype(auto) {
      return call_ignoring_nothing(FWD(f), FWD(xs)...);
    },
    FWD(t));
}

template <typename F, typename... Ts>
using result_of_ignoring_nothing_t
  = decltype(call_ignoring_nothing(std::declval<F>(), std::declval<Ts>()...));

template <typename F, typename... Ts, std::size_t... Is>
void enumerate_args_impl(std::index_sequence<Is...>, F&& f, Ts&&... xs)
{
  (f(std::integral_constant<std::size_t, Is>{}, FWD(xs)), ...);
}

template <typename F, typename... Ts>
void enumerate_args(F&& f, Ts&&... xs)
{
  enumerate_args_impl(std::index_sequence_for<Ts...>{}, FWD(f), FWD(xs)...);
}

template <typename T>
struct type_wrapper {
  using type = T;
};

template <typename T>
inline constexpr type_wrapper<T> type_wrapper_v{};

template <typename T>
using unwrap_type = typename std::decay_t<T>::type;

class bool_latch {
private:
  std::condition_variable _cv;
  std::mutex _mtx;
  bool _finished{false};

public:
  void count_down()
  {
    std::scoped_lock lk{_mtx};
    _finished = true;
    _cv.notify_all();
  }

  void wait()
  {
    std::unique_lock lk{_mtx};
    _cv.wait(lk, [this] { return _finished; });
  }
};

template <typename Parent, typename F>
class node;

template <typename Parent, typename... Fs>
class when_all;

class root {
  template <typename>
  friend class node_base;

public:
  // The `root` produces `nothing`.
  using output_type = nothing;

private:
  // When we are at the `root`, we cannot go "up" the chain anymore.
  // Therefore we being going "down".
  template <typename Scheduler, typename Child, typename... Children>
  void walk_up(Scheduler&& s, Child& c, Children&... cs) &
  {
    c.execute(s, nothing{}, cs...);
  }
};

template <typename Parent>
class child_of : public Parent {
public:
  using input_type = typename Parent::output_type;

protected:
  template <typename ParentFwd>
  child_of(ParentFwd&& p) : Parent{FWD(p)}
  {
  }

  auto& as_parent() noexcept
  {
    return static_cast<Parent&>(*this);
  }
};

template <typename Derived>
class node_base {
private:
  auto& as_derived() noexcept
  {
    return static_cast<Derived&>(*this);
  }

public:
  template <typename... FThens>
  auto then(FThens&&... f_thens) &&
  {
    if constexpr (sizeof...(FThens) == 1) {
      return node{std::move(as_derived()), FWD(f_thens)...};
    }
    else {
      return when_all{std::move(as_derived()), FWD(f_thens)...};
    }
  }

  template <typename Scheduler, typename... Children>
  void walk_up(Scheduler&& s, Children&... cs) &
  {
    as_derived().as_parent().walk_up(s, as_derived(), cs...);
  }

  template <typename Scheduler>
  decltype(auto) wait_and_get(Scheduler&& s) &&
  {
    typename Derived::output_type out;
    bool_latch l;

    auto f = std::move(as_derived()).then([&](auto&&... x) {
      ((out = FWD(x)), ...);
      l.count_down();
    });

    f.walk_up(s);

    l.wait();
    return out;
  }
};

template <typename Parent, typename F>
class node : private child_of<Parent>,
             private F,
             public node_base<node<Parent, F>> {
public:
  using typename child_of<Parent>::input_type;
  using output_type = result_of_ignoring_nothing_t<F&, input_type>;

  template <typename ParentFwd, typename FFwd>
  node(ParentFwd&& p, FFwd&& f) : child_of<Parent>{FWD(p)}, F{FWD(f)}
  {
  }

  using crtp_base_type = node_base<node<Parent, F>>;
  friend crtp_base_type;

  using crtp_base_type::then;
  using crtp_base_type::wait_and_get;
  using crtp_base_type::walk_up;

private:
  auto& as_f() noexcept
  {
    return static_cast<F&>(*this);
  }

public:
  template <typename Scheduler, typename Result>
  void execute(Scheduler&&, Result&& r) &
  {
    call_ignoring_nothing(as_f(), FWD(r));
  }

  template <typename Scheduler, typename Result, typename Child,
            typename... Children>
  void execute(Scheduler&& s, Result&& r, Child& c, Children&... cs) &
  {
    // `r` doesn't need to be stored inside the node here as it is used
    // to synchronously invoke `as_f()`.
    c.execute(s, call_ignoring_nothing(as_f(), FWD(r)), cs...);
  }
};

template <typename ParentFwd, typename FFwd>
node(ParentFwd&&, FFwd &&)->node<std::decay_t<ParentFwd>, std::decay_t<FFwd>>;

template <typename Parent>
class schedule : private child_of<Parent>, public node_base<schedule<Parent>> {
public:
  using typename child_of<Parent>::input_type;
  using output_type = nothing;

  template <typename ParentFwd>
  schedule(ParentFwd&& p) : child_of<Parent>{FWD(p)}
  {
  }

  using crtp_base_type = node_base<schedule<Parent>>;
  friend crtp_base_type;

  using crtp_base_type::then;
  using crtp_base_type::wait_and_get;
  using crtp_base_type::walk_up;

  template <typename Scheduler, typename Result, typename Child,
            typename... Children>
  void execute(Scheduler&& s, Result&&, Child& c, Children&... cs) &
  {
    s([&] { c.execute(s, nothing{}, cs...); });
  }
};

template <typename ParentFwd>
schedule(ParentFwd &&)->schedule<std::decay_t<ParentFwd>>;

template <typename T>
struct movable_atomic : std::atomic<T> {
  using std::atomic<T>::atomic;
  movable_atomic(movable_atomic&& rhs) : std::atomic<T>{rhs.load()}
  {
  }
};

template <typename T>
struct tuple_of_nothing_to_empty;

template <typename T>
struct tuple_of_nothing_to_empty<std::tuple<T>> {
  using type = std::tuple<T>;
};

template <>
struct tuple_of_nothing_to_empty<std::tuple<nothing>> {
  using type = std::tuple<>;
};

template <typename T>
using tuple_of_nothing_to_empty_t = typename tuple_of_nothing_to_empty<T>::type;

template <typename T>
struct adapt_tuple_of_nothing;

template <typename... Ts>
struct adapt_tuple_of_nothing<std::tuple<Ts...>> {
  using type = decltype(std::tuple_cat(
    std::declval<tuple_of_nothing_to_empty_t<std::tuple<Ts>>>()...));
};

template <typename T>
using adapt_tuple_of_nothing_t = typename adapt_tuple_of_nothing<T>::type;

template <typename Parent, typename... Fs>
class when_all : private child_of<Parent>,
                 private Fs...,
                 public node_base<when_all<Parent, Fs...>> {
public:
  using typename child_of<Parent>::input_type;
  using output_type
    = std::tuple<result_of_ignoring_nothing_t<Fs&, input_type&>...>;

  // TODO: should this be done, or should the tuple be applied to following
  // node? using output_type = adapt_tuple_of_nothing_t<
  //     std::tuple<result_of_ignoring_nothing_t<Fs&, input_type&>...>
  // >;

private:
  // TODO: the size of the entire computation might grow by a lot. Is it
  // possible to reuse this space for multiple nodes?
  movable_atomic<int> _left{sizeof...(Fs)};
  output_type _out;
  std::aligned_storage_t<sizeof(input_type), alignof(input_type)> _input_buf;

public:
  template <typename ParentFwd, typename... FFwds>
  when_all(ParentFwd&& p, FFwds&&... fs)
      : child_of<Parent>{FWD(p)}, Fs{FWD(fs)}...
  {
  }

  using crtp_base_type = node_base<when_all<Parent, Fs...>>;
  friend crtp_base_type;

  using crtp_base_type::then;
  using crtp_base_type::wait_and_get;
  using crtp_base_type::walk_up;

public:
  template <typename Scheduler, typename Result>
  void execute(Scheduler&& s, Result&& r) &
  {
    // TODO: what if `Result` is an lvalue reference?
    new (&_input_buf) input_type(FWD(r));

    enumerate_args(
      [&](auto i, auto t) {
        auto do_computation = [&] {
          call_ignoring_nothing(static_cast<unwrap_type<decltype(t)>&>(*this),
                                reinterpret_cast<input_type&>(_input_buf));

          if (_left.fetch_sub(1) == 1) {
            // TODO: make sure this destruction is correct, probably should not
            // destroy if storing lvalue reference
            reinterpret_cast<input_type&>(_input_buf).~input_type();
          }
        };

        if constexpr (i == sizeof...(Fs) - 1) {
          do_computation();
        }
        else {
          // `do_computation` has to be moved here as it will die at the end of
          // the `enumerate_args` lambda scope.
          s([g = std::move(do_computation)] { g(); });
        }
      },
      type_wrapper_v<Fs>...);
  }

  template <typename Scheduler, typename Result, typename Child,
            typename... Children>
  void execute(Scheduler&& s, Result&& r, Child& c, Children&... cs) &
  {
    // This is necessary as `r` only lives as long as `execute` is active on the
    // call stack. Computations might still be active when `execute` ends, even
    // if the last one is executed on the same thread. This is because the
    // scheduled computations might finish after the last one.
    // TODO: what if `Result` is an lvalue reference?
    new (&_input_buf) input_type(FWD(r));

    enumerate_args(
      [&](auto i, auto t) {
        auto do_computation = [&] {
          std::get<decltype(i){}>(_out) = call_ignoring_nothing(
            static_cast<unwrap_type<decltype(t)>&>(*this),
            reinterpret_cast<input_type&>(_input_buf));

          if (_left.fetch_sub(1) == 1) {
            // TODO: make sure this destruction is correct, probably should not
            // destroy if storing lvalue reference
            reinterpret_cast<input_type&>(_input_buf).~input_type();
            c.execute(
              s, _out,
              cs...); // TODO: apply the tuple here to pass multiple arguments
          }
        };

        if constexpr (i == sizeof...(Fs) - 1) {
          do_computation();
        }
        else {
          // `do_computation` has to be moved here as it will die at the end of
          // the `enumerate_args` lambda scope.
          s([g = std::move(do_computation)] { g(); });
        }
      },
      type_wrapper_v<Fs>...);
  }
};

template <typename ParentFwd, typename... FFwds>
when_all(ParentFwd&&, FFwds&&...)
  ->when_all<std::decay_t<ParentFwd>, std::decay_t<FFwds>...>;

template <typename... Fs>
auto initiate(Fs&&... fs)
{
  return schedule{root{}}.then(FWD(fs)...);
}

struct zero_alloc_thread_pool {
  template <typename F>
  void operator()(F&& f)
  {
    std::thread{FWD(f)}.detach();
  }
};

} // end of namespace BABYLON

#endif // BABYLON_CORE_CONTINUATIONS_ZERO_ALLOCATION_CONTINUATIONS_TIMER_H
