#ifndef BABYLON_CORE_FAST_FUNC_H
#define BABYLON_CORE_FAST_FUNC_H

#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

/**
 * A C++11 re-implementation of Don Clugston's original FastDelegate by Paúl
 * Jiménez (Ceniza).
 * refs:
 * http://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible
 * https://gist.github.com/yxbh/997d5a7791e3fe18e94f
 * https://github.com/yxbh/Cpp-Delegate-Library-Collection
 */

#ifdef _MSC_VER

namespace BABYLON {
namespace Internal {
class __single_inheritance AnyClass;
class AnyClass {
};
using AnyPtrThis = AnyClass*;
using AnyPtrFunc = void (AnyClass::*)();
template <typename TReturn = void, typename... TArgs>
using AnyPtrFuncT = TReturn (AnyClass::*)(TArgs...);
template <typename TReturn = void, typename... TArgs>
using AnyPtrStaticFuncT = TReturn (*)(TArgs...);

constexpr std::size_t SingleMemFuncPtrSize{sizeof(void (AnyClass::*)())};

template <class TOut, class TIn>
union HorribleUnion {
  TOut out;
  TIn in;
};
template <class TOut, class TIn>
inline TOut horrible_cast(TIn mIn) noexcept
{
  HorribleUnion<TOut, TIn> u;
  static_assert(sizeof(TIn) == sizeof(u) && sizeof(TIn) == sizeof(TOut),
                "Cannot use horrible_cast<>");
  u.in = mIn;
  return u.out;
}
template <class TOut, class TIn>
inline TOut unsafe_horrible_cast(TIn mIn) noexcept
{
  HorribleUnion<TOut, TIn> u;
  u.in = mIn;
  return u.out;
}

template <std::size_t TN>
struct SimplifyMemFunc {
  static constexpr std::size_t invalid_size_value = TN;
  template <class TThis, class TFunc>
  inline static AnyPtrThis convert(const TThis*, TFunc, AnyPtrFunc&) noexcept
  {
    static_assert(false,
                  "Unsupported member function pointer on this compiler");
    return 0;
  }
};
template <>
struct SimplifyMemFunc<SingleMemFuncPtrSize> {
  template <class TThis, class TFunc>
  inline static AnyPtrThis convert(const TThis* mThis, TFunc mFunc,
                                   AnyPtrFunc& mFuncOut) noexcept
  {
    mFuncOut = reinterpret_cast<AnyPtrFunc>(mFunc);
    return reinterpret_cast<AnyPtrThis>(const_cast<TThis*>(mThis));
  }
};

/**
 * Besides the standard conforming version above, the VC++ compiler has
 * variable function pointer size depending
 * on function type, which is is not standard conforming. The original
 * FastDelegate handled this but the FastFunc
 * ignored VC++ compliance issue. The FastDelegate solution is ported forward
 * into this version to make it work.
 * Note: using the /vmg flag could make things work but it also breaks delegate
 * equality comparison for!
 */
template <>
struct SimplifyMemFunc<SingleMemFuncPtrSize + sizeof(int)> {
  template <class TThis, class TFunc>
  inline static AnyPtrThis convert(const TThis* mThis, TFunc mFunc,
                                   AnyPtrFunc& mFuncOut) noexcept
  {
    union {
      TFunc func;
      struct {
        AnyPtrFunc funcaddress; // points to the actual member function
        int delta;              // #BYTES to be added to the 'this' pointer
      } s;
    } u;
    // Check that the horrible_cast will work
    static_assert(sizeof(mFunc) == sizeof(u.s), "Cannot use horrible_cast<>");
    u.func   = mFunc;
    mFuncOut = u.s.funcaddress;
    return reinterpret_cast<AnyClass*>(
      reinterpret_cast<char*>(const_cast<TThis*>(mThis)) + u.s.delta);
  }
};
struct MicrosoftVirtualMFP {
  void (AnyClass::*codeptr)(); // points to the actual member function
  int delta;                   // #bytes to be added to the 'this' pointer
  int vtable_index;            // or 0 if no virtual inheritance
};

struct GenericVirtualClass : virtual public AnyClass {
  typedef GenericVirtualClass* (GenericVirtualClass::*ProbePtrType)();
  GenericVirtualClass* GetThis()
  {
    return this;
  }
};
template <>
struct SimplifyMemFunc<SingleMemFuncPtrSize + 2 * sizeof(int)> {
  template <class TThis, class TFunc>
  inline static AnyPtrThis convert(const TThis* mThis, TFunc mFunc,
                                   AnyPtrFunc& mFuncOut) noexcept
  {
    union {
      TFunc func;
      AnyClass* (TThis::*ProbeFunc)();
      MicrosoftVirtualMFP s;
    } u;
    u.func   = mFunc;
    mFuncOut = reinterpret_cast<AnyPtrFunc>(u.s.codeptr);
    union {
      GenericVirtualClass::ProbePtrType virtfunc;
      MicrosoftVirtualMFP s;
    } u2;
    // Check that the horrible_cast<>s will work
    static_assert(sizeof(mFunc) == sizeof(u.s)
                    && sizeof(mFunc) == sizeof(u.ProbeFunc)
                    && sizeof(u2.virtfunc) == sizeof(u2.s),
                  "Cannot use horrible_cast<>");
    // Unfortunately, taking the address of a MF prevents it from being inlined,
    // so
    // this next line can't be completely optimised away by the compiler.
    u2.virtfunc = &GenericVirtualClass::GetThis;
    u.s.codeptr = u2.s.codeptr;
    return (const_cast<TThis*>(mThis)->*u.ProbeFunc)();
  }
};
template <>
struct SimplifyMemFunc<SingleMemFuncPtrSize + 3 * sizeof(int)> {
  template <class TThis, class TFunc>
  inline static AnyPtrThis convert(const TThis* mThis, TFunc mFunc,
                                   AnyPtrFunc& mFuncOut) noexcept
  {
    // The member function pointer is 16 bytes long. We can't use a normal cast,
    // but
    // we can use a union to do the conversion.
    union {
      TFunc func;
      // In VC++ and ICL, an unknown_inheritance member pointer
      // is internally defined as:
      struct {
        AnyPtrFunc funcaddress; // points to the actual member function
        int delta;              // #bytes to be added to the 'this' pointer
        int vtordisp;           // #bytes to add to 'this' to find the vtable
        int vtable_index;       // or 0 if no virtual inheritance
      } s;
    } u;
    // Check that the horrible_cast will work
    static_assert(sizeof(TFunc) == sizeof(u.s), "Cannot use horrible_cast<>");
    u.func            = mFunc;
    mFuncOut          = u.s.funcaddress;
    int virtual_delta = 0;
    if (u.s.vtable_index) { // Virtual inheritance is used
      // First, get to the vtable.
      // It is 'vtordisp' bytes from the start of the class.
      const int* vtable = *reinterpret_cast<const int* const*>(
        reinterpret_cast<const char*>(mThis) + u.s.vtordisp);

      // 'vtable_index' tells us where in the table we should be looking.
      virtual_delta
        = u.s.vtordisp
          + *reinterpret_cast<const int*>(reinterpret_cast<const char*>(vtable)
                                          + u.s.vtable_index);
    }
    // The int at 'virtual_delta' gives us the amount to add to 'this'.
    // Finally we can add the three components together. Phew!
    return reinterpret_cast<AnyClass*>(
      reinterpret_cast<char*>(const_cast<TThis*>(mThis)) + u.s.delta
      + virtual_delta);
  }
};

template <typename TReturn, typename... TArgs>
struct Closure {
private:
  using PtrFuncT       = AnyPtrFuncT<TReturn, TArgs...>;
  using PtrStaticFuncT = AnyPtrStaticFuncT<TReturn, TArgs...>;
  AnyPtrThis ptrThis{nullptr};
  AnyPtrFunc ptrFunction{nullptr};

public:
  template <class TThis, class TFunc>
  inline void bind(TThis* mThis, TFunc mFunc) noexcept
  {
    ptrThis = BABYLON::Internal::SimplifyMemFunc<sizeof(mFunc)>::convert(
      mThis, mFunc, ptrFunction);
  }
  template <class TThis, class TInvoker>
  inline void bind(TThis* mThis, TInvoker mInvoker,
                   PtrStaticFuncT mFunc) noexcept
  {
    if (mFunc == nullptr)
      ptrFunction = nullptr;
    else
      bind(mThis, mInvoker);
    ptrThis = horrible_cast<AnyPtrThis>(mFunc);
  }

  inline bool operator==(::std::nullptr_t) const noexcept
  {
    return ptrThis == nullptr && ptrFunction == nullptr;
  }
  inline bool operator==(const Closure& mRhs) const noexcept
  {
    return ptrThis == mRhs.ptrThis && ptrFunction == mRhs.ptrFunction;
  }
  inline bool operator==(PtrStaticFuncT mPtr) const noexcept
  {
    return mPtr == nullptr ?
             *this == nullptr :
             mPtr == reinterpret_cast<PtrStaticFuncT>(getStaticFunc());
  }
  inline bool operator!=(::std::nullptr_t) const noexcept
  {
    return !operator==(nullptr);
  }
  inline bool operator!=(const Closure& mRhs) const noexcept
  {
    return !operator==(mRhs);
  }
  inline bool operator!=(PtrStaticFuncT mPtr) const noexcept
  {
    return !operator==(mPtr);
  }
  inline bool operator<(const Closure& mRhs) const
  {
    return ptrThis != mRhs.ptrThis ?
             ptrThis < mRhs.ptrThis :
             std::memcmp(&ptrFunction, &mRhs.ptrFunction, sizeof(ptrFunction))
               < 0;
  }
  inline bool operator>(const Closure& mRhs) const
  {
    return !operator<(mRhs);
  }

  inline std::size_t getHash() const noexcept
  {
    return reinterpret_cast<std::size_t>(ptrThis)
           ^ Internal::unsafe_horrible_cast<std::size_t>(ptrFunction);
  }
  inline AnyPtrThis getPtrThis() const noexcept
  {
    return ptrThis;
  }
  inline PtrFuncT getPtrFunction() const noexcept
  {
    return reinterpret_cast<PtrFuncT>(ptrFunction);
  }
  inline PtrStaticFuncT getStaticFunc() const noexcept
  {
    return horrible_cast<PtrStaticFuncT>(this);
  }
};

template <typename TReturn, typename... TArgs>
class FastFuncImpl {
protected:
  using PtrStaticFuncT = AnyPtrStaticFuncT<TReturn, TArgs...>;

private:
  Closure<TReturn, TArgs...> closure;
  inline TReturn invokeStaticFunc(TArgs... mArgs) const
  {
    return (*(closure.getStaticFunc()))(std::forward<TArgs>(mArgs)...);
  }

protected:
  template <class TThis, class TFunc>
  inline void bind(TThis* mThis, TFunc mFunc) noexcept
  {
    closure.bind(mThis, mFunc);
  }
  template <class TFunc>
  inline void bind(TFunc mFunc) noexcept
  {
    closure.bind(this, &FastFuncImpl::invokeStaticFunc, mFunc);
  }

public:
  inline FastFuncImpl() noexcept = default;
  inline FastFuncImpl(::std::nullptr_t) noexcept
  {
  }
  inline FastFuncImpl(PtrStaticFuncT mFunc) noexcept
  {
    bind(mFunc);
  }
  template <typename X, typename Y>
  inline FastFuncImpl(X* mThis, Y mFunc) noexcept
  {
    bind(mThis, mFunc);
  }

  inline FastFuncImpl& operator=(PtrStaticFuncT mFunc) noexcept
  {
    bind(mFunc);
  }
  inline TReturn operator()(TArgs... mArgs) const
  {
    return (closure.getPtrThis()->*(closure.getPtrFunction()))(
      std::forward<TArgs>(mArgs)...);
  }

  inline bool operator==(::std::nullptr_t) const noexcept
  {
    return closure == nullptr;
  }
  inline bool operator==(const FastFuncImpl& mImpl) const noexcept
  {
    return closure == mImpl.closure;
  }
  inline bool operator==(PtrStaticFuncT mFuncPtr) const noexcept
  {
    return closure == mFuncPtr;
  }
  inline bool operator!=(::std::nullptr_t) const noexcept
  {
    return !operator==(nullptr);
  }
  inline bool operator!=(const FastFuncImpl& mImpl) const noexcept
  {
    return !operator==(mImpl);
  }
  inline bool operator!=(PtrStaticFuncT mFuncPtr) const noexcept
  {
    return !operator==(mFuncPtr);
  }
  inline bool operator<(const FastFuncImpl& mImpl) const
  {
    return closure < mImpl.closure;
  }
  inline bool operator>(const FastFuncImpl& mImpl) const
  {
    return !operator<(mImpl);
  }
};
} // namespace Internal

template <typename T>
struct MemFuncToFunc;
template <typename TReturn, typename TThis, typename... TArgs>
struct MemFuncToFunc<TReturn (TThis::*)(TArgs...) const> {
  using Type = TReturn (*)(TArgs...);
};

#define ENABLE_IF_CONV_TO_FUN_PTR(x)                                           \
  typename ::std::enable_if<                                                   \
    ::std::is_constructible<typename MemFuncToFunc<decltype(                   \
                              &::std::decay<x>::type::operator())>::Type,      \
                            x>::value>::type* = nullptr
#define ENABLE_IF_NOT_CONV_TO_FUN_PTR(x)                                       \
  typename ::std::enable_if<                                                   \
    !::std::is_constructible<typename MemFuncToFunc<decltype(                  \
                               &::std::decay<x>::type::operator())>::Type,     \
                             x>::value>::type* = nullptr
#define ENABLE_IF_SAME_TYPE(x, y)                                              \
  typename = typename ::std::enable_if<                                        \
    !::std::is_same<x, typename ::std::decay<y>::type>{}>::type

template <typename T>
class FastFunc;
template <typename TReturn, typename... TArgs>
class FastFunc<TReturn(TArgs...)>
    : public Internal::FastFuncImpl<TReturn, TArgs...> {
private:
  using BaseType = Internal::FastFuncImpl<TReturn, TArgs...>;
  std::shared_ptr<void> storage;
  template <typename T>
  inline static void funcDeleter(void* mPtr)
  {
    static_cast<T*>(mPtr)->~T();
    operator delete(mPtr);
  }

public:
  //        using BaseType::BaseType;
  inline FastFunc(::std::nullptr_t) noexcept
  {
  }
  inline FastFunc(PtrStaticFuncT mFunc) noexcept : BaseType(mFunc)
  {
  }
  template <typename X, typename Y>
  inline FastFunc(X* mThis, Y mFunc) noexcept : BaseType(mThis, mFunc)
  {
  }

  inline FastFunc() noexcept = default;

  template <typename TFunc, ENABLE_IF_SAME_TYPE(FastFunc, TFunc)>
  inline FastFunc(TFunc&& mFunc, ENABLE_IF_CONV_TO_FUN_PTR(TFunc))
  {
    using FuncType = typename ::std::decay<TFunc>::type;
    this->bind(&mFunc, &FuncType::operator());
  }
  template <typename TFunc, ENABLE_IF_SAME_TYPE(FastFunc, TFunc)>
  inline FastFunc(TFunc&& mFunc, ENABLE_IF_NOT_CONV_TO_FUN_PTR(TFunc))
      : storage(operator new(sizeof(TFunc)),
                funcDeleter<typename std::decay<TFunc>::type>)
  {
    using FuncType = typename std::decay<TFunc>::type;
    new (storage.get()) FuncType(std::forward<TFunc>(mFunc));
    this->bind(storage.get(), &FuncType::operator());
  }
};

#undef ENABLE_IF_CONV_TO_FUN_PTR
#undef ENABLE_IF_NOT_CONV_TO_FUN_PTR
#undef ENABLE_IF_SAME_TYPE
} // namespace BABYLON

#else

namespace BABYLON {
namespace Internal {
class AnyClass;
using AnyPtrThis = AnyClass*;
using AnyPtrFunc = void (AnyClass::*)();
template <typename TReturn = void, typename... TArgs>
using AnyPtrFuncT = TReturn (AnyClass::*)(TArgs...);
template <typename TReturn = void, typename... TArgs>
using AnyPtrStaticFuncT = TReturn (*)(TArgs...);

constexpr std::size_t SingleMemFuncPtrSize{sizeof(void (AnyClass::*)())};

template <class TOut, class TIn>
union HorribleUnion {
  TOut out;
  TIn in;
};
template <class TOut, class TIn>
inline TOut horrible_cast(TIn mIn) noexcept
{
  HorribleUnion<TOut, TIn> u;
  static_assert(sizeof(TIn) == sizeof(u) && sizeof(TIn) == sizeof(TOut),
                "Cannot use horrible_cast<>");
  u.in = mIn;
  return u.out;
}
template <class TOut, class TIn>
inline TOut unsafe_horrible_cast(TIn mIn) noexcept
{
  HorribleUnion<TOut, TIn> u;
  u.in = mIn;
  return u.out;
}

template <std::size_t TN>
struct SimplifyMemFunc {
  template <class TThis, class TFunc>
  inline static AnyPtrThis convert(const TThis*, TFunc, AnyPtrFunc&) noexcept
  {
    static_assert(TN - 100,
                  "Unsupported member function pointer on this compiler");
    return 0;
  }
};
template <>
struct SimplifyMemFunc<SingleMemFuncPtrSize> {
  template <class TThis, class TFunc>
  inline static AnyPtrThis convert(const TThis* mThis, TFunc mFunc,
                                   AnyPtrFunc& mFuncOut) noexcept
  {
    mFuncOut = reinterpret_cast<AnyPtrFunc>(mFunc);
    return reinterpret_cast<AnyPtrThis>(const_cast<TThis*>(mThis));
  }
};

template <typename TReturn, typename... TArgs>
struct Closure {
private:
  using PtrFuncT       = AnyPtrFuncT<TReturn, TArgs...>;
  using PtrStaticFuncT = AnyPtrStaticFuncT<TReturn, TArgs...>;
  AnyPtrThis ptrThis{nullptr};
  AnyPtrFunc ptrFunction{nullptr};

public:
  template <class TThis, class TFunc>
  inline void bind(TThis* mThis, TFunc mFunc) noexcept
  {
    ptrThis
      = SimplifyMemFunc<sizeof(mFunc)>::convert(mThis, mFunc, ptrFunction);
  }
  template <class TThis, class TInvoker>
  inline void bind(TThis* mThis, TInvoker mInvoker,
                   PtrStaticFuncT mFunc) noexcept
  {
    if (mFunc == nullptr)
      ptrFunction = nullptr;
    else
      bind(mThis, mInvoker);
    ptrThis = horrible_cast<AnyPtrThis>(mFunc);
  }

  inline bool operator==(::std::nullptr_t) const noexcept
  {
    return ptrThis == nullptr && ptrFunction == nullptr;
  }
  inline bool operator==(const Closure& mRhs) const noexcept
  {
    return ptrThis == mRhs.ptrThis && ptrFunction == mRhs.ptrFunction;
  }
  inline bool operator==(PtrStaticFuncT mPtr) const noexcept
  {
    return mPtr == nullptr ?
             *this == nullptr :
             mPtr == reinterpret_cast<PtrStaticFuncT>(getStaticFunc());
  }
  inline bool operator!=(::std::nullptr_t) const noexcept
  {
    return !operator==(nullptr);
  }
  inline bool operator!=(const Closure& mRhs) const noexcept
  {
    return !operator==(mRhs);
  }
  inline bool operator!=(PtrStaticFuncT mPtr) const noexcept
  {
    return !operator==(mPtr);
  }
  inline bool operator<(const Closure& mRhs) const
  {
    return ptrThis != mRhs.ptrThis ?
             ptrThis < mRhs.ptrThis :
             std::memcmp(&ptrFunction, &mRhs.ptrFunction, sizeof(ptrFunction))
               < 0;
  }
  inline bool operator>(const Closure& mRhs) const
  {
    return !operator<(mRhs);
  }

  inline std::size_t getHash() const noexcept
  {
    return reinterpret_cast<std::size_t>(ptrThis)
           ^ Internal::unsafe_horrible_cast<std::size_t>(ptrFunction);
  }
  inline AnyPtrThis getPtrThis() const noexcept
  {
    return ptrThis;
  }
  inline PtrFuncT getPtrFunction() const noexcept
  {
    return reinterpret_cast<PtrFuncT>(ptrFunction);
  }
  inline PtrStaticFuncT getStaticFunc() const noexcept
  {
    return horrible_cast<PtrStaticFuncT>(this);
  }
};

template <typename TReturn, typename... TArgs>
class FastFuncImpl {
private:
  using PtrStaticFuncT = AnyPtrStaticFuncT<TReturn, TArgs...>;
  Closure<TReturn, TArgs...> closure;
  inline TReturn invokeStaticFunc(TArgs... mArgs) const
  {
    return (*(closure.getStaticFunc()))(std::forward<TArgs>(mArgs)...);
  }

protected:
  template <class TThis, class TFunc>
  inline void bind(TThis* mThis, TFunc mFunc) noexcept
  {
    closure.bind(mThis, mFunc);
  }
  template <class TFunc>
  inline void bind(TFunc mFunc) noexcept
  {
    closure.bind(this, &FastFuncImpl::invokeStaticFunc, mFunc);
  }

public:
  inline FastFuncImpl() noexcept = default;
  inline FastFuncImpl(::std::nullptr_t) noexcept
  {
  }
  inline FastFuncImpl(PtrStaticFuncT mFunc) noexcept
  {
    bind(mFunc);
  }
  template <typename X, typename Y>
  inline FastFuncImpl(X* mThis, Y mFunc) noexcept
  {
    bind(mThis, mFunc);
  }

  inline FastFuncImpl& operator=(PtrStaticFuncT mFunc) noexcept
  {
    bind(mFunc);
  }
  inline TReturn operator()(TArgs... mArgs) const
  {
    return (closure.getPtrThis()->*(closure.getPtrFunction()))(
      std::forward<TArgs>(mArgs)...);
  }

  inline bool operator==(::std::nullptr_t) const noexcept
  {
    return closure == nullptr;
  }
  inline bool operator==(const FastFuncImpl& mImpl) const noexcept
  {
    return closure == mImpl.closure;
  }
  inline bool operator==(PtrStaticFuncT mFuncPtr) const noexcept
  {
    return closure == mFuncPtr;
  }
  inline bool operator!=(::std::nullptr_t) const noexcept
  {
    return !operator==(nullptr);
  }
  inline bool operator!=(const FastFuncImpl& mImpl) const noexcept
  {
    return !operator==(mImpl);
  }
  inline bool operator!=(PtrStaticFuncT mFuncPtr) const noexcept
  {
    return !operator==(mFuncPtr);
  }
  inline bool operator<(const FastFuncImpl& mImpl) const
  {
    return closure < mImpl.closure;
  }
  inline bool operator>(const FastFuncImpl& mImpl) const
  {
    return !operator<(mImpl);
  }
};
} // namespace Internal

template <typename T>
struct MemFuncToFunc;
template <typename TReturn, typename TThis, typename... TArgs>
struct MemFuncToFunc<TReturn (TThis::*)(TArgs...) const> {
  using Type = TReturn (*)(TArgs...);
};

#define ENABLE_IF_CONV_TO_FUN_PTR(x)                                           \
  typename std::enable_if<std::is_constructible<                               \
    typename MemFuncToFunc<decltype(&std::decay<x>::type::operator())>::Type,  \
    x>::value>::type* = nullptr
#define ENABLE_IF_NOT_CONV_TO_FUN_PTR(x)                                       \
  typename std::enable_if<!std::is_constructible<                              \
    typename MemFuncToFunc<decltype(&std::decay<x>::type::operator())>::Type,  \
    x>::value>::type* = nullptr
#define ENABLE_IF_SAME_TYPE(x, y)                                              \
  typename = typename std::enable_if<                                          \
    !std::is_same<x, typename std::decay<y>::type>{}>::type

template <typename T>
class FastFunc;
template <typename TReturn, typename... TArgs>
class FastFunc<TReturn(TArgs...)>
    : public Internal::FastFuncImpl<TReturn, TArgs...> {
private:
  using BaseType = Internal::FastFuncImpl<TReturn, TArgs...>;
  std::shared_ptr<void> storage;
  template <typename T>
  inline static void funcDeleter(void* mPtr)
  {
    static_cast<T*>(mPtr)->~T();
    operator delete(mPtr);
  }

public:
  using BaseType::BaseType;

  inline FastFunc() noexcept = default;

  template <typename TFunc, ENABLE_IF_SAME_TYPE(FastFunc, TFunc)>
  inline FastFunc(TFunc&& mFunc, ENABLE_IF_CONV_TO_FUN_PTR(TFunc))
  {
    using FuncType = typename std::decay<TFunc>::type;
    this->bind(&mFunc, &FuncType::operator());
  }
  template <typename TFunc, ENABLE_IF_SAME_TYPE(FastFunc, TFunc)>
  inline FastFunc(TFunc&& mFunc, ENABLE_IF_NOT_CONV_TO_FUN_PTR(TFunc))
      : storage(operator new(sizeof(TFunc)),
                funcDeleter<typename std::decay<TFunc>::type>)
  {
    using FuncType = typename std::decay<TFunc>::type;
    new (storage.get()) FuncType(std::forward<TFunc>(mFunc));
    this->bind(storage.get(), &FuncType::operator());
  }
};

#undef ENABLE_IF_CONV_TO_FUN_PTR
#undef ENABLE_IF_NOT_CONV_TO_FUN_PTR
#undef ENABLE_IF_SAME_TYPE
} // namespace BABYLON

#endif

#endif // end of BABYLON_CORE_FAST_FUNC_H