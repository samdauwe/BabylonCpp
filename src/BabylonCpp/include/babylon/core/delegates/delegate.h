/*
  Copyright (C) 2017 by Sergey A Kryukov: derived work
  http://www.SAKryukov.org
  http://www.codeproject.com/Members/SAKryukov
  https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed

  Based on original work by Sergey Ryazanov:
  "The Impossibly Fast C++ Delegates", 18 Jul 2005
  https://www.codeproject.com/articles/11015/the-impossibly-fast-c-delegates

  MIT license:
  http://en.wikipedia.org/wiki/MIT_License

  Original publication:
   https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed
*/

#pragma once
#include <babylon/core/delegates/delegate_base.h>

namespace SA {

template <typename T>
class delegate;
template <typename T>
class multicast_delegate;

template <typename RET, typename... PARAMS>
class delegate<RET(PARAMS...)> final : private delegate_base<RET(PARAMS...)> {
public:
  delegate() = default;

  [[nodiscard]] bool isNull() const
  {
    return invocation.stub == nullptr;
  }
  bool operator==(void* ptr) const
  {
    return (ptr == nullptr) && this->isNull();
  } // operator ==
  bool operator!=(void* ptr) const
  {
    return (ptr != nullptr) || (!this->isNull());
  } // operator !=
  operator bool() const
  {
    return !this->isNull();
  } // operator bool()

  delegate(const delegate& another)
  {
    another.invocation.Clone(invocation);
  }

  template <typename LAMBDA>
  delegate(const LAMBDA& lambda)
  {
    assign(const_cast<LAMBDA*>(&lambda), lambda_stub<LAMBDA>);
  } // delegate

  delegate& operator=(const delegate& another)
  {
    another.invocation.Clone(invocation);
    return *this;
  } // operator =

  template <typename LAMBDA> // template instantiation is not needed, will be
  // deduced (inferred):
  delegate& operator=(const LAMBDA& instance)
  {
    assign(const_cast<LAMBDA*>(&instance), lambda_stub<LAMBDA>);
    return *this;
  } // operator =

  void reset()
  {
    invocation.object = nullptr;
    invocation.stub   = nullptr;
  } // reset

  bool operator==(const delegate& another) const
  {
    return invocation == another.invocation;
  }
  bool operator!=(const delegate& another) const
  {
    return invocation != another.invocation;
  }

  bool operator==(const multicast_delegate<RET(PARAMS...)>& another) const
  {
    return another == (*this);
  }
  bool operator!=(const multicast_delegate<RET(PARAMS...)>& another) const
  {
    return another != (*this);
  }

  template <class T, RET (T::*TMethod)(PARAMS...)>
  static delegate create(T* instance)
  {
    return delegate(instance, method_stub<T, TMethod>);
  } // create

  template <class T, RET (T::*TMethod)(PARAMS...) const>
  static delegate create(T const* instance)
  {
    return delegate(const_cast<T*>(instance), const_method_stub<T, TMethod>);
  } // create

  template <RET (*TMethod)(PARAMS...)>
  static delegate create()
  {
    return delegate(nullptr, function_stub<TMethod>);
  } // create

  template <typename LAMBDA>
  static delegate create(const LAMBDA& instance)
  {
    return delegate(const_cast<LAMBDA*>(&instance), lambda_stub<LAMBDA>);
  } // create

  RET operator()(PARAMS... arg) const
  {
    return (*invocation.stub)(invocation.object, arg...);
  } // operator()

private:
  delegate(void* anObject, typename delegate_base<RET(PARAMS...)>::stub_type aStub)
  {
    invocation.object = anObject;
    invocation.stub   = aStub;
  } // delegate

  void assign(void* anObject, typename delegate_base<RET(PARAMS...)>::stub_type aStub)
  {
    this->invocation.object = anObject;
    this->invocation.stub   = aStub;
  } // assign

  template <class T, RET (T::*TMethod)(PARAMS...)>
  static RET method_stub(void* this_ptr, PARAMS... params)
  {
    T* p = static_cast<T*>(this_ptr);
    return (p->*TMethod)(params...);
  } // method_stub

  template <class T, RET (T::*TMethod)(PARAMS...) const>
  static RET const_method_stub(void* this_ptr, PARAMS... params)
  {
    T* const p = static_cast<T*>(this_ptr);
    return (p->*TMethod)(params...);
  } // const_method_stub

  template <RET (*TMethod)(PARAMS...)>
  static RET function_stub(void* /*this_ptr*/, PARAMS... params)
  {
    return (TMethod)(params...);
  } // function_stub

  template <typename LAMBDA>
  static RET lambda_stub(void* this_ptr, PARAMS... arg)
  {
    auto* p = static_cast<LAMBDA*>(this_ptr);
    return (p->operator())(arg...);
  } // lambda_stub

  friend class multicast_delegate<RET(PARAMS...)>;
  typename delegate_base<RET(PARAMS...)>::InvocationElement invocation;

}; // class delegate

} /* namespace SA */
