#ifndef BABYLON_CORE_JSON_H
#define BABYLON_CORE_JSON_H

#include <babylon/babylon_global.h>
#include <babylon/utils/picojson.h>

namespace BABYLON {
namespace Json {

inline string_t Parse(Json::value& parsedData, const string_t& json)
{
  return picojson::parse(parsedData, json);
}

template <typename T>
inline picojson::value ToJson(const std::deque<T>& items)
{
  picojson::array arr;

  for (const auto& i : items) {
    arr.push_back(picojson::value(i));
  }

  return picojson::value(arr);
}

template <typename T>
inline std::deque<T>& FromJson(std::deque<T>& items,
                               const picojson::value& input)
{
  if (input.is<picojson::array>()) {
    const auto& array = input.get<picojson::array>();
    for (const auto& a : array) {
      items.push_back(a.get<T>());
    }
  }

  return items;
}

template <class T,
          typename ::std::enable_if<::std::is_same<T, int>::value, int>::type
          = 0>
inline std::pair<string_t, picojson::value> Pair(const string_t& name,
                                                 int value)
{
  return ::std::make_pair(name, picojson::value(static_cast<double>(value)));
}

template <class T, typename ::std::enable_if<
                     ::std::is_same<T, unsigned int>::value, unsigned int>::type
                   = 0>
inline std::pair<string_t, picojson::value> Pair(const string_t& name,
                                                 unsigned int value)
{
  return ::std::make_pair(name, picojson::value(static_cast<double>(value)));
}

template <
  class T,
  typename ::std::enable_if<::std::is_same<T, size_t>::value, size_t>::type = 0>
inline std::pair<string_t, picojson::value> Pair(const string_t& name,
                                                 size_t value)
{
  return ::std::make_pair(name, picojson::value(static_cast<double>(value)));
}

template <class T,
          typename ::std::enable_if<!::std::is_same<T, int>::value, int>::type
          = 0>
inline std::pair<string_t, picojson::value> Pair(const string_t& name,
                                                 const T& value)
{
  return ::std::make_pair(name, picojson::value(value));
}

template <class T,
          typename ::std::enable_if<::std::is_same<T, int>::value, int>::type
          = 0>
inline Json::value NameValuePair(const string_t& name, int value)
{
  return picojson::value(
    picojson::object{{"name", picojson::value(name)},
                     {"value", picojson::value(static_cast<double>(value))}});
}

template <class T,
          typename ::std::enable_if<!::std::is_same<T, int>::value, int>::type
          = 0>
inline Json::value NameValuePair(const string_t& name, const T& value)
{
  return picojson::value(picojson::object{{"name", picojson::value(name)},
                                          {"value", picojson::value(value)}});
}

template <typename T>
inline T GetNumber(const picojson::value& v, const string_t& key,
                   T defaultValue)
{
  if (v.contains(key)) {
    return static_cast<T>(v.get(key).get<double>());
  }
  else {
    return defaultValue;
  }
}

inline bool GetBool(const picojson::value& v, const string_t& key,
                    bool defaultValue = false)
{
  if (v.contains(key)) {
    return v.get(key).get<bool>();
  }
  else {
    return defaultValue;
  }
}

inline string_t GetString(const picojson::value& v, const string_t& key,
                          const string_t& defaultValue = "")
{
  if (v.contains(key)) {
    return v.get(key).get<string_t>();
  }
  else {
    return defaultValue;
  }
}

inline Json::array GetArray(const picojson::value& v, const string_t& key)
{
  if (v.contains(key) && v.get(key).is<Json::array>()) {
    return v.get(key).get<Json::array>();
  }
  return Json::array();
}

template <typename T>
inline vector_t<T> ToArray(const picojson::value& v, const string_t& key)
{
  vector_t<T> array;
  if (v.contains(key) && (v.get(key).is<picojson::array>())) {
    array.reserve(v.get(key).get<picojson::array>().size());
    for (auto& element : v.get(key).get<picojson::array>()) {
      array.emplace_back(static_cast<T>(element.get<double>()));
    }
  }
  return array;
}

inline vector_t<string_t> ToStringVector(const picojson::value& v,
                                         const string_t& key)
{
  vector_t<string_t> stringVector;
  if (v.contains(key) && (v.get(key).is<picojson::array>())) {
    stringVector.reserve(v.get(key).get<picojson::array>().size());
    for (auto& element : v.get(key).get<picojson::array>()) {
      stringVector.emplace_back(element.get<string_t>());
    }
  }
  return stringVector;
}

} // namespace Json
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_STRING_H
