#ifndef BABYLON_CORE_STRING_H
#define BABYLON_CORE_STRING_H

#include <babylon/babylon_stl.h>
#include <babylon/babylon_stl_util.h>

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
 * @brief Returns the number of occurrences of the string str in the other
 * string s.
 * @param s. The source string.
 * @param str. The string to search for and count.
 * @return A number, representing the number of occurrences of the string str.
 */
inline size_t count(const std::string& s, const std::string& str)
{
  size_t count = 0;
  size_t nPos  = s.find(str, 0); // first occurrence
  while (nPos != std::string::npos) {
    ++count;
    nPos = s.find(str, nPos + 1);
  }
  return count;
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
 * @brief Escapes a character.
 * @param character The character to escape.
 * @return The escaped character.
 */
inline std::string escape(char character)
{
  static const std::unordered_map<char, std::string> escapedSpecialCharacters
    = {{'.', "\\."}, {'|', "\\|"}, {'*', "\\*"}, {'?', "\\?"},  {'+', "\\+"},
       {'(', "\\("}, {')', "\\)"}, {'{', "\\{"}, {'}', "\\}"},  {'[', "\\["},
       {']', "\\]"}, {'^', "\\^"}, {'$', "\\$"}, {'\\', "\\\\"}};

  auto it = escapedSpecialCharacters.find(character);
  if (it == escapedSpecialCharacters.end()) {
    return std::string(1, character);
  }

  return it->second;
}

/**
 * @brief Escapes a string.
 * @param s The string to escape.
 * @return The escaped string.
 */
inline std::string escape(const std::string& s)
{
  std::ostringstream ostream;
  std::for_each(s.begin(), s.end(), [&ostream](const char character) {
    ostream << escape(character);
  });
  return ostream.str();
}

/**
 * @brief Escapes a list of string.
 * @param ss The list of string to escape.
 * @return The escaped list of string.
 */
inline std::vector<std::string> escape(const std::vector<std::string>& ss)
{
  return stl_util::map(ss, [](const std::string& s) { return escape(s); });
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
 * @brief Returns the position of the first occurrence of a specified value in a
 * string.
 * @param src The string to search in.
 * @param searchvalue The string to search for.
 * @param start At which position to start the search. Default 0.
 * @return A Number, representing the position where the specified searchvalue
 * occurs for the first time, or -1 if it never occurs.
 */
inline int indexOf(const std::string& src, const std::string& searchvalue,
                   size_t start = 0)
{
  int index                  = -1;
  std::string::size_type loc = src.find(searchvalue, start);
  if (loc != std::string::npos) {
    index = static_cast<int>(loc);
  }
  return index;
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
 * @brief Returns the position of the last occurrence of a specified value in a
 * string.
 * @param src The string to search in.
 * @param searchvalue The string to search for.
 * @param start The position where to start the search (searching backwards). If
 * omitted, the default value is the length of the string.
 * @return A Number, representing the position where the specified searchvalue
 * occurs for the first time, or -1 if it never occurs.
 */
inline int lastIndexOf(const std::string& src, const std::string& searchvalue,
                       size_t start)
{
  int index                  = -1;
  std::string::size_type loc = src.rfind(searchvalue, start);
  if (loc != std::string::npos) {
    index = static_cast<int>(loc);
  }
  return index;
}

inline int lastIndexOf(const std::string& src, const std::string& searchvalue)
{
  return lastIndexOf(src, searchvalue, src.size());
}

/**
 * @brief Returns the nth character from a char array.
 */
template <unsigned N>
inline char nthChar(const char (&arr)[N], unsigned i)
{
  if (i >= N) {
    return '\0';
  }

  return arr[i];
}

/**
 * Pads a string with a number of occurrences of some character to a certain
 * width.
 */
template <typename T>
void pad(std::basic_string<T>& s, typename std::basic_string<T>::size_type n,
         T c)
{
  if (n > s.length()) {
    s.append(n - s.length(), c);
  }
}

/**
 * @brief String formatting like sprintf.
 */
template <typename... Args>
inline std::string printf(const std::string& format, Args... args)
{
  size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(buf.get(), buf.get() + size - 1);
}

/**
 * @brief Inserts the contents of s2 at offset 0 in s1.
 * @param s1 The string to update.
 * @param s2 The string to prepend.
 */
inline void pushFront(std::string& s1, const std::string& s2)
{
  s1.insert(0, s2);
}

/**
 * @brief Removes a substring from a string.
 * @param s. The input string.
 * @param subStr. The substring to remove.
 * @return Reference to the updated string.
 */
inline std::string& removeSubstring(std::string& s, const std::string& subStr)
{
  auto n = subStr.length();
  for (auto i = s.find(subStr); i != std::string::npos; i = s.find(subStr)) {
    s.erase(i, n);
  }
  return s;
}

/**
 * @brief Uses a regular expression to perform substitution on a sequence of
 * characters.
 * @param source The input character sequence.
 * @param reSearch The regular expression that will be matched against the input
 * sequence.
 * @param replace The regex replacement format string.
 * @return Result of the replacement.
 */
inline std::string regexReplace(const std::string& source,
                                const std::string& reSearch,
                                const std::string& replace)
{
  std::string result;
  std::regex regex(reSearch);
  std::regex_replace(std::back_inserter(result), source.begin(), source.end(),
                     regex, replace);
  return result;
}

/**
 * @brief Returns a new string with a specified number of copies of the string
 * it was called on.
 * @param str The string to repeat.
 * @param count The number of times the original string value should be repeated
 * in the new string.
 * @return A String, a new string containing copies of the original string.
 */
inline std::string repeat(const std::string& str, size_t count)
{
  std::string result;
  result.reserve(count * str.length());
  for (size_t i = 0; i < count; ++i) {
    result += str;
  }
  return result;
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

inline std::string slice(const std::string& s, size_t start = 0)
{
  return s.substr(start);
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
 * @brief Converts a string to a number.
 * @param str Required. The string to convert.
 * @return A number, representing the numeric value of the string.
 */
template <typename T>
inline T toNumber(const std::string& str)
{
  T value;
  std::stringstream ss(str);
  ss >> value;
  return value;
}

/**
 * @brief Converts a number to a string.
 * @param number Required. The number to convert.
 * @return A string, representing the string value of the number.
 */
template <typename T>
inline std::string toString(const T& number)
{
  std::stringstream ss;
  ss << number;
  return ss.str();
}

/**
 * @brief Upper-cases the first letter of each word.
 * @param str. The string to titlecase.
 */
inline std::string& toTitleCase(std::string& str)
{
  auto it = str.begin();
  *it     = static_cast<char>(toupper(*it));
  for (; it != str.end() - 1; ++it) {
    if (*it == ' ') {
      *(it + 1) = static_cast<char>(toupper(*(it + 1)));
    }
  }
  return str;
}

/**
 * @brief Strip whitespace from the beginning of a string.
 * @param str The input string.
 * @return Returns a string with whitespace stripped from the beginning of str.
 */
inline std::string& trimLeft(std::string& str)
{
  auto it2 = std::find_if(str.begin(), str.end(), [](char ch) {
    return !std::isspace<char>(ch, std::locale::classic());
  });
  str.erase(str.begin(), it2);
  return str;
}

/**
 * @brief Strip whitespace from the end of a string.
 * @param str The input string.
 * @return A string with whitespace stripped from the end of str.
 */
inline std::string& trimRight(std::string& str)
{
  auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) {
    return !std::isspace<char>(ch, std::locale::classic());
  });
  str.erase(it1.base(), str.end());
  return str;
}

/**
 * @brief Removes leading and trailing whitespace characters from a string.
 * @param str The input string.
 * @return The string with whitespace stripped from the beginning end of str.
 */
inline std::string& trim(std::string& str)
{
  return trimLeft(trimRight(str));
}

/**
 * @brief Removes leading and trailing whitespace characters from a string.
 * @param str The input string.
 * @return A copy of the string with whitespace stripped from the beginning end
 * of str.
 */
inline std::string trimCopy(const std::string& str)
{
  auto s = str;
  return trimLeft(trimRight(s));
}

} // end of namespace String
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_STRING_H
