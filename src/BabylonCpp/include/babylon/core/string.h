#ifndef BABYLON_CORE_STRING_H
#define BABYLON_CORE_STRING_H

#include <babylon/babylon_stl.h>

namespace BABYLON {
namespace String {

/**
 * Variadic template for concatenating a string.
 */
inline void toString(std::ostream& /*o*/)
{
}

template <class T, class... Ts>
inline void toString(std::ostream& o, const T& t0, const Ts&... ts)
{
  o << t0;
  toString(o, ts...);
}

/**
 * @brief Joins two or more values.
 * @param t0, t1, ..., tn Required. The values to be joined.
 * @return A new String, containing the text of the values.
 */
template <class... T>
inline std::string concat(const T&... t0)
{
  std::ostringstream o;
  toString(o, t0...);
  return o.str();
}

/**
 * @brief Checks if a string contains the other substring.
 * @param s. The string to check.
 * @param toCheck. The substring to search for.
 * @return A boolean value, whether or not the string contains the substring.
 */
inline bool contains(const std::string& s, const std::string& toCheck)
{
  return (s.find(toCheck) != std::string::npos);
}

/**
 * Variadic template for concatenating a string.
 */
inline void charCodeToString(std::ostream& /*o*/)
{
}

template <class T, class... Ts>
inline void charCodeToString(std::ostream& o, const T& t0, const Ts&... ts)
{
  o << static_cast<char>(t0);
  charCodeToString(o, ts...);
}

/**
 * @brief Checks if a string starts with the other string.
 * @param s. The string to check.
 * @param prefix. The substring to search for.
 * @return A boolean value, whether or not the string starts with the prefix
 * string.
 */
inline bool startsWith(const std::string& s, const std::string& prefix)
{
  if (prefix.size() > s.size()) {
    return false;
  }
  return std::equal(prefix.begin(), prefix.end(), s.begin());
}

/**
 * @brief Checks if a string ends with the other string.
 * @param s. The string to check.
 * @param postfix. The substring to search for.
 * @return A boolean value, whether or not the string ends with the postfix
 * string.
 */
inline bool endsWith(const std::string& s, const std::string& postfix)
{
  if (postfix.size() > s.size()) {
    return false;
  }
  return std::equal(postfix.rbegin(), postfix.rend(), s.rbegin());
}

/**
 * @brief Converts Unicode values to characters.
 * @param t0, t1, ..., tn Required. One or more Unicode values to be converted.
 * @return A String, representing the character(s) representing the specified
 * unicode number(s).
 */
template <class... T>
inline std::string fromCharCode(const T&... t0)
{
  std::ostringstream o;
  charCodeToString(o, t0...);
  return o.str();
}

/**
 * @brief Returns whether or not the arhument is a digit.
 * @param x Required. The value to check.
 * @return A boolean value, whether or not the argument is a digit.
 */
template <typename T>
inline bool isDigit(T x)
{
  std::string s;
  std::regex e("^-?\\d+");
  std::stringstream ss;
  ss << x;
  ss >> s;
  if (std::regex_match(s, e)) {
    return true;
  }
  else {
    return false;
  }
}

/**
 * @brief Joins the list using the delimiter character.
 * @param v Required. The list to join.
 * @param delim Required. The delimiter character.
 * @return A new String, containing the joined list.
 */
template <typename T>
inline std::string join(const T& v, char delim)
{
  std::ostringstream s;
  for (const auto& i : v) {
    if (&i != &v[0]) {
      s << delim;
    }
    s << i;
  }
  return s.str();
}

/**
 * @brief Strip whitespace from the beginning of a string.
 * @param str The input string.
 * @return Returns a string with whitespace stripped from the beginning of str.
 */
inline std::string& ltrim(std::string& str)
{
  auto it2 = std::find_if(str.begin(), str.end(), [](char ch) {
    return !std::isspace<char>(ch, std::locale::classic());
  });
  str.erase(str.begin(), it2);
  return str;
}

/**
 * @brief Searches a string for a specified value and replaces the specified
 * values.
 * @param source The source string where the specified value(s) should replaced
 * by the new value.
 * @param search Required. The value that will be replaced by the replace value.
 * @param replace Required. The value to replace the search value with.
 */
inline void replaceInPlace(std::string& source, const std::string& search,
                           const std::string& replace)
{
  size_t pos = 0;
  while ((pos = source.find(search, pos)) != std::string::npos) {
    source.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

/**
 * @brief Strip whitespace from the end of a string.
 * @param str The input string.
 * @return Returns a string with whitespace stripped from the end of str.
 */
inline std::string& rtrim(std::string& str)
{
  auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) {
    return !std::isspace<char>(ch, std::locale::classic());
  });
  str.erase(it1.base(), str.end());
  return str;
}

/**
 * @brief Splits a string into an array of substrings.
 * @param value Required. The string to split.
 * @param separator Required. Specifies the character to use for splitting the
 * string.
 * @return An Array, containing the splitted values.
 */
inline std::vector<std::string> split(const std::string& value, char separator)
{
  std::vector<std::string> result;
  std::string::size_type p = 0;
  std::string::size_type q;
  while ((q = value.find(separator, p)) != std::string::npos) {
    result.emplace_back(value, p, q - p);
    p = q + 1;
  }
  result.emplace_back(value, p);
  return result;
}

/**
 * @brief Converts a string to lowercase letters.
 * @param source Required. The string to convert.
 * @return A String, representing the value of a string converted to lowercase.
 */
inline std::string toLowerCase(const std::string& source)
{
  std::string lcs = source;
  std::transform(lcs.begin(), lcs.end(), lcs.begin(), ::tolower);
  return lcs;
}

/**
 * @brief Converts a string to uppercase letters.
 * @param source Required. The string to convert.
 * @return A String, representing the value of a string converted to uppercase.
 */
inline std::string toUpperCase(const std::string& source)
{
  std::string ucs = source;
  std::transform(ucs.begin(), ucs.end(), ucs.begin(), ::toupper);
  return ucs;
}

/**
 * @brief Removes leading and trailing whitespace characters from a string.
 * @param str The input string.
 * @return Returns the string with whitespace stripped from the beginning end of
 * str.
 */
inline std::string& trim(std::string& str)
{
  return ltrim(rtrim(str));
}

/**
 * @brief Removes leading and trailing whitespace characters from a string.
 * @param str The input string.
 * @return Returns a copy of the string with whitespace stripped from the
 * beginning end of str.
 */
inline std::string trimCopy(const std::string& str)
{
  auto s = str;
  return ltrim(rtrim(s));
}

} // end of namespace String
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_STRING_H
