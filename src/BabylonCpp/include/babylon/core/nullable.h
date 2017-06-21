#ifndef BABYLON_CORE_NULLABLE_H
#define BABYLON_CORE_NULLABLE_H

#include <cstddef>
#include <utility>

namespace BABYLON {

template <typename T>
class Nullable final {

private:
  bool _hasValue;
  T _value;

public:
  const T& value;

public:
  Nullable() : _hasValue(false), _value(T()), value(_value)
  {
  }

  Nullable(T value) : _hasValue(true), _value(value), value(_value)
  {
  }

  Nullable(const Nullable& rhs)
      : _hasValue(rhs._hasValue), _value(rhs._value), value(_value)
  {
  }

  Nullable(std::nullptr_t /*nullpointer*/)
      : _hasValue(false), _value(T()), value(_value)
  {
  }

  Nullable(Nullable&& rhs)
      : _hasValue(std::move(rhs._hasValue))
      , _value(std::move(rhs._value))
      , value(_value)
  {
    rhs._hasValue = false;
  }

  void resetValue()
  {
    _hasValue = false;
  }

  bool hasValue() const
  {
    return _hasValue;
  }

  bool isNull() const
  {
    return !_hasValue;
  }

  const T& operator*() const
  {
    return value;
  }

  operator bool() const
  {
    return _hasValue;
  }

  friend void swap(Nullable& n0, Nullable& n1)
  {
    std::swap(n0, n1);
  }

  const Nullable<T>& operator=(const Nullable<T>& value)
  {
    _hasValue = value._hasValue;
    _value    = value._value;
    return *this;
  }

  const Nullable<T>& operator=(const T& value)
  {
    _hasValue = true;
    _value    = value;
    return *this;
  }

  const Nullable<T>& operator=(std::nullptr_t /*nullpointer*/)
  {
    _hasValue = false;
    _value    = T();
    return *this;
  }

  template <typename T2>
  friend bool operator==(const Nullable<T2>& op1, const Nullable<T2>& op2);

  template <typename T2>
  friend bool operator==(const Nullable<T2>& op, const T2& value);

  template <typename T2>
  friend bool operator==(const T2& value, const Nullable<T2>& op);

  template <typename T2>
  friend bool operator==(const Nullable<T2>& op, std::nullptr_t nullpointer);

  template <typename T2>
  friend bool operator==(std::nullptr_t nullpointer, const Nullable<T2>& op);

}; // end of class Nullable

template <typename T>
bool operator==(const Nullable<T>& op1, const Nullable<T>& op2)
{
  if (op1._hasValue != op2._hasValue) {
    return false;
  }

  if (op1._hasValue) {
    return op1._value == op2._value;
  }
  else {
    return true;
  }
}

template <typename T>
bool operator==(const Nullable<T>& op, const T& value)
{
  if (!op._hasValue) {
    return false;
  }

  return op._value == value;
}

template <typename T>
bool operator==(const T& value, const Nullable<T>& op)
{
  if (!op._hasValue) {
    return false;
  }

  return op._value == value;
}

template <typename T>
bool operator==(const Nullable<T>& op, std::nullptr_t /*nullpointer*/)
{
  if (op._hasValue) {
    return false;
  }
  else {
    return true;
  }
}

template <typename T>
bool operator==(std::nullptr_t /*nullpointer*/, const Nullable<T>& op)
{
  if (op._hasValue) {
    return false;
  }
  else {
    return true;
  }
}

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_NULLABLE_H
