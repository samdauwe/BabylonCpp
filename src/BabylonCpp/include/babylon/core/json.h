#ifndef BABYLON_CORE_JSON_H
#define BABYLON_CORE_JSON_H

#include <babylon/babylon_global.h>
#include <babylon/utils/picojson.h>

namespace BABYLON {
namespace Json {

template <class T,
          typename std::enable_if<std::is_same<T, int>::value, int>::type = 0>
static inline std::pair<std::string, picojson::value>
Pair(const std::string& name, int value)
{
  return std::make_pair(name, picojson::value(static_cast<double>(value)));
}

template <class T, typename std::enable_if<std::is_same<T, unsigned int>::value,
                                           unsigned int>::type
                   = 0>
static inline std::pair<std::string, picojson::value>
Pair(const std::string& name, unsigned int value)
{
  return std::make_pair(name, picojson::value(static_cast<double>(value)));
}

template <class T,
          typename std::enable_if<std::is_same<T, size_t>::value, size_t>::type
          = 0>
static inline std::pair<std::string, picojson::value>
Pair(const std::string& name, size_t value)
{
  return std::make_pair(name, picojson::value(static_cast<double>(value)));
}

template <class T,
          typename std::enable_if<!std::is_same<T, int>::value, int>::type = 0>
static inline std::pair<std::string, picojson::value>
Pair(const std::string& name, const T& value)
{
  return std::make_pair(name, picojson::value(value));
}

template <class T,
          typename std::enable_if<std::is_same<T, int>::value, int>::type = 0>
static inline Json::value NameValuePair(const std::string& name, int value)
{
  return picojson::value(
    picojson::object{{"name", picojson::value(name)},
                     {"value", picojson::value(static_cast<double>(value))}});
}

template <class T,
          typename std::enable_if<!std::is_same<T, int>::value, int>::type = 0>
static inline Json::value NameValuePair(const std::string& name, const T& value)
{
  return picojson::value(picojson::object{{"name", picojson::value(name)},
                                          {"value", picojson::value(value)}});
}

template <typename T>
static inline T GetNumber(const picojson::value& v, const std::string& key,
                          T defaultValue)
{
  if (v.contains(key)) {
    return static_cast<T>(v.get(key).get<double>());
  }
  else {
    return defaultValue;
  }
}
static inline bool GetBool(const picojson::value& v, const std::string& key,
                           bool defaultValue)
{
  if (v.contains(key)) {
    return v.get(key).get<bool>();
  }
  else {
    return defaultValue;
  }
}
static inline std::string GetString(const picojson::value& v,
                                    const std::string& key,
                                    std::string defaultValue)
{
  if (v.contains(key)) {
    return v.get(key).get<std::string>();
  }
  else {
    return defaultValue;
  }
}
template <typename T>
static inline std::vector<T> ToArray(const picojson::value& v,
                                     const std::string& key)
{
  std::vector<T> array;
  if (v.contains(key) && (v.get(key).is<picojson::array>())) {
    array.reserve(v.get(key).get<picojson::array>().size());
    for (auto& element : v.get(key).get<picojson::array>()) {
      array.emplace_back(static_cast<T>(element.get<double>()));
    }
  }
  return array;
}

} // end of namespace json
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_STRING_H
