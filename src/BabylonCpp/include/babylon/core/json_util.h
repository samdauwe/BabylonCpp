#ifndef BABYLON_CORE_JSON_UTIL_H
#define BABYLON_CORE_JSON_UTIL_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace BABYLON {
namespace json_util {

inline json has_key(const json& o, const std::string& key)
{
  return (o.find(key) != o.end());
}

template <typename T = bool>
inline T get_bool(const json& j, const std::string& key, T defaultValue = T())
{
  if (!j.is_null() && has_key(j, key) && j[key].is_boolean()) {
    return j[key].get<T>();
  }
  else {
    return defaultValue;
  }
}

template <typename T>
inline T get_number(const json& j, const std::string& key, T defaultValue = T())
{
  if (!j.is_null() && has_key(j, key) && j[key].is_number()) {
    return j[key].get<T>();
  }
  else {
    return defaultValue;
  }
}

template <typename T = std::string>
inline T get_string(const json& j, const std::string& key, T defaultValue = T())
{
  if (!j.is_null() && has_key(j, key) && j[key].is_string()) {
    // return j[key].get<T>();
    return defaultValue;
  }
  else {
    return defaultValue;
  }
}

template <typename T>
inline std::vector<T> get_array(const json& j, const std::string& key)
{
  std::vector<T> v;
  if (!j.is_null() && has_key(j, key) && j[key].is_array() && !j[key].empty()) {
    v = j[key].get<std::vector<T>>();
  }

  return v;
}

} // end of namespace json_util
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_JSON_UTIL_H
