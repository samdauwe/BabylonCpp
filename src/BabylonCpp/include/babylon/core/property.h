#ifndef BABYLON_CORE_PROPERTY_H
#define BABYLON_CORE_PROPERTY_H

#if 0
The MIT License (MIT)

Copyright (c) 2015 Gašper Ažman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
#endif

#include <utility> // Used for std::forward

namespace BABYLON {

namespace Meta {
template <typename T>
struct type_ {
  using type = T;
};

template <typename T>
type_<T> type_c;

template <typename Type, Type Value>
struct value_ {
  using type = Type;
  static constexpr type value{Value};
  constexpr operator type() const
  {
    return Value;
  }
  static constexpr type get()
  {
    return Value;
  }
};

template <typename Type, Type Value>
value_<Type, Value> value_c;
} // end of namespace Meta

// only call in class scope!
#define PROPERTY_WITH_STORAGE(type, name, host, getter, setter)                \
  struct PROPERTY__TAG_NAME(name);                                             \
  ::Property::                                                                 \
    rw_property<::Meta::type_<host::PROPERTY__TAG_NAME(name)>, host,           \
                ::Meta::value_<decltype(&host::getter), &host::getter>,        \
                ::Meta::value_<decltype(&host::setter), &host::setter>, type>  \
      name;                                                                    \
  auto static constexpr PROPERTY__FUNC_NAME(                                   \
    decltype(::Property::Impl::type_tag(name)))                                \
  {                                                                            \
    return &host::name;                                                        \
  }                                                                            \
  /* so that we can have the missing semicolon... */                           \
  static_assert(true, "")

// end define
#define PROPERTY(name, host, getter, setter)                                   \
  PROPERTY_WITH_STORAGE(char, name, host, getter, setter)
#define PROPERTY__TAG_NAME(name) _property__##name##_prop_tag
#define PROPERTY__FUNC_NAME _property__offset_get

namespace Property {
namespace Impl {

template <typename T>
auto constexpr type_tag(T const&)
{
  using Meta::type_c;
  return type_c<typename T::type_tag>;
}

/**
 * @brief Alternative implementation of offsetof, but with a member pointer
 * instead of a name. At this point, the host type is fully defined.
 */
template <typename Host, typename PointerToMemberType>
auto constexpr offset_of(PointerToMemberType member_ptr)
{
  return reinterpret_cast<std::size_t>(&(static_cast<Host*>(0)->*member_ptr));
}

template <typename Host, typename Property>
auto constexpr offset_of_property(Property const* property)
{
  auto const tag        = type_tag(*property);
  auto const member_ptr = Host::PROPERTY__FUNC_NAME(tag);
  return offset_of<Host>(member_ptr);
}

/**
 * @param property should be the 'this' pointer of an rw_property or
 * compatible class.
 */
template <typename Host, typename Property>
constexpr Host const& get_host(Property const* property)
{
  auto const raw_property_ptr = reinterpret_cast<char const*>(property);
  auto const raw_host_ptr
    = raw_property_ptr - offset_of_property<Host>(property);
  auto const host_ptr = reinterpret_cast<Host const*>(raw_host_ptr);
  return *host_ptr;
}

/**
 * @brief Cast const back in, then cast it out again, so main functions remain
 * const.
 */
template <typename Host, typename Property>
Host& get_host(Property* property)
{
  auto const const_property_ptr = const_cast<Property const*>(property);
  auto const& const_host_ref    = get_host<Host>(const_property_ptr);
  return const_cast<Host&>(const_host_ref);
}

} // end of namespace Impl

template <typename TypeTag, typename Host, typename Getter, typename Setter,
          typename ValueType>
struct rw_property {
  using getter     = Getter;
  using setter     = Setter;
  using type_tag   = TypeTag;
  using host       = Host;
  using value_type = ValueType;

  friend host;

  operator decltype(auto)() const
  {
    using namespace BABYLON::Property::Impl;
    return (get_host<host>(this).*getter::value)();
  }

  template <typename X>
  decltype(auto) operator=(X&& x)
  {
    using namespace BABYLON::Property::Impl;
    return (get_host<host>(this).*setter::value)(std::forward<X>(x));
  }

private:
  ValueType value;
  /// disallow copying for non-friend users of the class - this doesn't have a
  /// value, but if copied, it can get really, really bad (stack corruption).
  rw_property()        = default;
  rw_property& operator=(rw_property const&) = default;
  rw_property(rw_property const&)            = default;
  ~rw_property()                             = default;
  rw_property& operator=(rw_property&&) = default;
  rw_property(rw_property&&)            = default;
}; // end of struct rw_property

} // end of namespace Property
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_PROPERTY_H
