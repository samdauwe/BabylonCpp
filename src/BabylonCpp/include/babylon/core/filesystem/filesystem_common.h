#ifndef BABYLON_CORE_FILESYSTEM_FILESYSTEM_COMMON_H
#define BABYLON_CORE_FILESYSTEM_FILESYSTEM_COMMON_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Filesystem {

/**
 * @brief Returns the extension of the filename.
 * @param filename A filename or file path.
 * @return The extension of the file or an empty string when the filename has no
 * extension.
 */
inline string_t extension(const string_t& filename)
{
  auto pos = filename.find_last_of(".");
  if (pos == string_t::npos) {
    return "";
  }
  return filename.substr(pos + 1);
}

/**
 * @brief Returns the base directory of a file of subdirectory.
 * @param filepath The file path.
 * @return The base directory of a file of subdirectory.
 */
inline string_t baseDir(const string_t& filepath)
{
  auto pos = filepath.find_last_of("/\\");
  if (pos != string_t::npos) {
    return filepath.substr(0, pos);
  }
  return "";
}

/**
 * @brief Returns the filename of a path.
 * @param filepath The file path.
 * @return The filename of a path.
 */
inline string_t baseName(const string_t& filepath)
{
  auto pos = filepath.find_last_of("/\\");
  if (pos != string_t::npos) {
    return filepath.substr(pos + 1);
  }
  return "";
}

/**
 * @brief Join two path components.
 * @param path0 First path components.
 * @param path1 Second path components.
 * @return Concatenation of two path components with path separator character in
 * between.
 */
template <typename T>
inline T joinPath(const T& path0, const T& path1)
{
  if (path0.empty()) {
    return path1;
  }
  else {
    // check '/'
    char lastChar0  = *path0.rbegin();
    char firstChar1 = path1.empty() ? '\0' : *path1.cbegin();
    if ((lastChar0 != '/') && (firstChar1 != '/')) {
      return path0 + string_t("/") + path1;
    }
    else {
      return path0 + path1;
    }
  }
}

template <typename T, typename... Args>
inline T joinPath(const T& path0, const T& path1, Args... args)
{
  return joinPath(joinPath(path0, path1), args...);
}

/**
 * @brief Reads the file with the given filename into a string.
 * @param filename The path of the file to read from.
 * @return The contents of the file or an empty string in case the content could
 * not be read.
 */
inline string_t readFileContents(const char* filename)
{
  string_t contents;
  ::std::ifstream in(filename, ::std::ios::in | ::std::ios::binary);
  if (in) {
    in.seekg(0, ::std::ios::end);
    contents.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0, ::std::ios::beg);
    in.read(&contents[0], static_cast<long>(contents.size()));
    in.close();
  }
  return contents;
}

/**
 * @brief Reads the file with the given filename into a vector of strings.
 * @param filename The path of the file to read from.
 * @return The contents of the file or an empty vector in case the content could
 * not be read.
 */
inline vector_t<string_t> readFileLines(const char* filename)
{
  vector_t<string_t> lines;
  ::std::ifstream in(filename, ::std::ios::in | ::std::ios::binary);
  if (in) {
    for (string_t line; ::std::getline(in, line);) {
      lines.emplace_back(line);
    }
  }
  return lines;
}

/**
 * @brief Removes the extension from a filename.
 * @param filename. The filename.
 * @return The filename with the extension removes.
 */
inline string_t removeExtension(const string_t& filename)
{
  auto p = filename.find_last_of('.');
  return p > 0 && p != string_t::npos ? filename.substr(0, p) : filename;
}

/**
 * @brief Standardizes a path: use forward slashes only, end with slash.
 * @param path The path to standardize.
 * @return The standardized path.
 */
inline string_t standardizePath(const string_t& path)
{
  string_t _path = path;
  ::std::replace(_path.begin(), _path.end(), '\\', '/');
  if (_path.back() != '/') {
    _path += '/';
  }

  return _path;
}

/**
 * @brief Writes the given string contents to a file.
 * @param filename The path of the file to read from.
 * @param contents The contents to write to the file.
 * @return Whether or not the content was written to the file.
 */
inline bool writeFileContents(const char* filename, const string_t& contents)
{
  bool writtentoFile = false;
  ::std::ofstream out(filename, ::std::ios::out);
  if (out) {
    out.write(contents.c_str(), static_cast<long>(contents.size()));
    out.close();
    writtentoFile = true;
  }
  return writtentoFile;
}

/**
 * @brief Writes the given vector of strings to a file.
 * @param filename The path of the file to read from.
 * @param contents The contents to write to the file.
 * @return Whether or not the content was written to the file.
 */
inline bool writeFileLines(const char* filename,
                           const vector_t<string_t>& lines)
{
  bool writtentoFile = false;
  ::std::ofstream out(filename, ::std::ios::out);
  if (out) {
    for (auto& line : lines) {
      out.write(line.c_str(), static_cast<long>(line.size()));
      out.write("\n", 1);
    }
    out.close();
    writtentoFile = true;
  }
  return writtentoFile;
}

} // end of namespace Filesystem
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_FILESYSTEM_FILESYSTEM_COMMON_H
