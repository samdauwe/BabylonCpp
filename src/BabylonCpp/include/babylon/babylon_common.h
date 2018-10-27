#ifndef BABYLON_COMMON_H
#define BABYLON_COMMON_H

// Containers library
#include <vector>

// Time functions
#include <chrono>

// Standard library header
#include <cstdint>

// Strings library
#include <string>

#include <optional>

namespace BABYLON {

// -- Type support -- //

// Time types
using millisecond_t  = std::chrono::duration<uint64_t, std::ratio<1, 1000>>;
using milliseconds_t = std::chrono::milliseconds;
using microsecond_t  = std::chrono::duration<uint64_t, std::ratio<1, 1000000>>;
using microseconds_t = std::chrono::microseconds;
using high_res_clock_t      = std::chrono::high_resolution_clock;
using high_res_time_point_t = std::chrono::time_point<high_res_clock_t>;
using system_clock_t        = std::chrono::system_clock;
using system_time_point_t   = std::chrono::time_point<system_clock_t>;

// Byte type
using byte = unsigned char;

// -- Array types -- //
// 8-bit twos complement signed integer
using Int8Array = std::vector<int8_t>;
// 8-bit unsigned integer
using Uint8Array = std::vector<uint8_t>;
// 16-bit twos complement signed integer
using Int16Array = std::vector<int16_t>;
// 16-bit unsigned integer
using Uint16Array = std::vector<uint16_t>;
// 32-bit twos complement signed integer
using Int32Array = std::vector<int32_t>;
// 32-bit unsigned integer
using Uint32Array = std::vector<uint32_t>;
// 64-bit twos complement signed integer
using Int64Array = std::vector<int64_t>;
// 64-bit unsigned integer
using Uint64Array = std::vector<uint64_t>;
// 32-bit IEEE floating point number
using Float32Array = std::vector<float>;
// 64-bit IEEE floating point number
using Float64Array = std::vector<double>;

// Indices array alias
using IndicesArray = std::vector<uint32_t>;

// An ArrayBuffer object is used to represent a generic, fixed-length raw binary
// data buffer.
using ArrayBuffer = std::vector<uint8_t>;

// -- Read-Only Properties -- //

template <typename C, typename T, typename E = void>
class ReadOnlyProperty; // undefined

template <typename C, typename T>
class ReadOnlyProperty<
  C, T,
  typename std::enable_if<std::is_fundamental<T>::value
                          || std::is_same<std::string, T>::value>::type> {
public:
  using TGetter = T (C::*)() const;

  ReadOnlyProperty(C* propObject, TGetter propGetter)
      : _object{propObject}, _getter{propGetter}
  {
  }

  ReadOnlyProperty& operator=(const ReadOnlyProperty&) = delete;
  ReadOnlyProperty(const ReadOnlyProperty&)            = delete;

  operator T() const
  {
    return (_object->*_getter)();
  }

  T operator()() const
  {
    return (_object->*_getter)();
  }

private:
  C* const _object;
  TGetter const _getter;
};

template <typename C, typename T>
class ReadOnlyProperty<
  C, T,
  typename std::enable_if<!std::is_fundamental<T>::value
                          && !std::is_same<std::string, T>::value>::type> {
public:
  using TGetter = T& (C::*)();

  ReadOnlyProperty(C* propObject, TGetter propGetter)
      : _object{propObject}, _getter{propGetter}
  {
  }

  ReadOnlyProperty& operator=(const ReadOnlyProperty&) = delete;
  ReadOnlyProperty(const ReadOnlyProperty&)            = delete;

  operator const T&() const
  {
    return (_object->*_getter)();
  }

  operator T&()
  {
    return (_object->*_getter)();
  }

  const T& operator()() const
  {
    return (_object->*_getter)();
  }

  T& operator()()
  {
    return (_object->*_getter)();
  }

private:
  C* /*const*/ _object;
  TGetter const _getter;
};

// -- Write-Only Properties -- //

template <typename C, typename T, typename E = void>
class WriteOnlyProperty; // undefined

template <typename C, typename T>
class WriteOnlyProperty<
  C, T,
  typename std::enable_if<std::is_fundamental<T>::value
                          || std::is_same<std::string, T>::value>::type> {
public:
  using TSetter = void (C::*)(T);

  WriteOnlyProperty(C* propObject, TSetter propSetter)
      : _object{propObject}, _setter{propSetter}
  {
  }

  WriteOnlyProperty& operator=(const WriteOnlyProperty&) = delete;
  WriteOnlyProperty(const WriteOnlyProperty&)            = delete;

  C& operator=(T theValue)
  {
    (_object->*_setter)(theValue);
    return *_object;
  }

private:
  C* const _object;
  TSetter const _setter;
};

template <typename C, typename T>
class WriteOnlyProperty<
  C, T,
  typename std::enable_if<!std::is_fundamental<T>::value
                          && !std::is_same<std::string, T>::value>::type> {
public:
  using TSetter = void (C::*)(const T&);

  WriteOnlyProperty(C* propObject, TSetter propSetter)
      : _object{propObject}, _setter{propSetter}
  {
  }

  WriteOnlyProperty& operator=(const WriteOnlyProperty&) = delete;
  WriteOnlyProperty(const WriteOnlyProperty&)            = delete;

  C& operator=(const T& newValue)
  {
    (_object->*_setter)(newValue);
    return *_object;
  }

private:
  C* const _object;
  TSetter const _setter;
};

// -- Read-Write Properties -- //

template <typename C, typename T, typename E = void>
class Property; // undefined

template <typename C, typename T>
class Property<
  C, T,
  typename std::enable_if<std::is_fundamental<T>::value
                          || std::is_same<std::string, T>::value>::type> {
public:
  using TGetter = T (C::*)() const;
  using TSetter = void (C::*)(T);

  Property(C* propObject, T C::*attribute)
      : _object{propObject}
      , _attribute{attribute}
      , _getter{nullptr}
      , _setter{nullptr}
  {
  }

  Property(C* propObject, TGetter propGetter, TSetter propSetter)
      : _object{propObject}
      , _attribute{nullptr}
      , _getter{propGetter}
      , _setter{propSetter}
  {
  }

  Property& operator=(const Property&) = delete;
  Property(const Property&)            = delete;

  operator T() const
  {
    return _attribute ? (_object->*_attribute) : (_object->*_getter)();
  }

  T operator()() const
  {
    return _attribute ? (_object->*_attribute) : (_object->*_getter)();
  }

  C& operator=(T newValue)
  {
    if (_attribute) {
      (_object->*_attribute) = newValue;
    }
    else {
      (_object->*_setter)(newValue);
    }
    return *_object;
  }

private:
  C* const _object;
  T C::*_attribute;
  TGetter const _getter;
  TSetter const _setter;
};

template <typename C, typename T>
class Property<
  C, T,
  typename std::enable_if<!std::is_fundamental<T>::value
                          && !std::is_same<std::string, T>::value>::type> {
public:
  using TGetter = T& (C::*)();
  using TSetter = void (C::*)(const T&);

  Property(C* propObject, T C::*attribute)
      : _object{propObject}
      , _attribute{attribute}
      , _getter{nullptr}
      , _setter{nullptr}
  {
  }

  Property(C* propObject, TGetter propGetter, TSetter propSetter)
      : _object{propObject}
      , _attribute{nullptr}
      , _getter{propGetter}
      , _setter{propSetter}
  {
  }

  Property& operator=(const Property&) = delete;
  Property(const Property&)            = delete;

  operator T&()
  {
    return _attribute ? (_object->*_attribute) : (_object->*_getter)();
  }

  operator const T&() const
  {
    return _attribute ? (_object->*_attribute) : (_object->*_getter)();
  }

  T& operator()()
  {
    return _attribute ? (_object->*_attribute) : (_object->*_getter)();
  }

  const T& operator()() const
  {
    return _attribute ? (_object->*_attribute) : (_object->*_getter)();
  }

  C& operator=(const T& newValue)
  {
    if (_attribute) {
      (_object->*_attribute) = newValue;
    }
    else {
      (_object->*_setter)(newValue);
    }
    return *_object;
  }

private:
  C* const _object;
  T C::*_attribute;
  TGetter const _getter;
  TSetter const _setter;
};

} // end of namespace BABYLON

#endif // end of BABYLON_COMMON_H
