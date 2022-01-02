#ifndef BABYLON_CORE_FILESYSTEM_FILESYSTEM_COMMON_H
#define BABYLON_CORE_FILESYSTEM_FILESYSTEM_COMMON_H

#include <algorithm>
#include <fstream>
#include <ios>
#include <iterator>
#include <string>
#include <vector>

namespace BABYLON {

// An ArrayBuffer object is used to represent a generic, fixed-length raw binary
// data buffer.
using ArrayBuffer = std::vector<uint8_t>;

namespace Filesystem {

/**
 * @brief Returns the extension of the filename.
 * @param filename A filename or file path.
 * @return The extension of the file or an empty string when the filename has no
 * extension.
 */
inline std::string extension(const std::string& filename)
{
  auto pos = filename.find_last_of(".");
  if (pos == std::string::npos) {
    return "";
  }
  return filename.substr(pos + 1);
}

/**
 * @brief Returns the base directory of a file of subdirectory.
 * @param filepath The file path.
 * @return The base directory of a file of subdirectory.
 */
inline std::string baseDir(const std::string& filepath)
{
  auto pos = filepath.find_last_of("/\\");
  if (pos != std::string::npos) {
    return filepath.substr(0, pos);
  }
  return "";
}

/**
 * @brief Returns the filename of a path.
 * @param filepath The file path.
 * @return The filename of a path.
 */
inline std::string baseName(const std::string& filepath)
{
  auto pos = filepath.find_last_of("/\\");
  if (pos != std::string::npos) {
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
      return path0 + std::string("/") + path1;
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
inline std::string readFileContents(const char* filename)
{
  std::string contents;
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in) {
    in.seekg(0, std::ios::end);
    contents.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0, std::ios::beg);
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
inline std::vector<std::string> readFileLines(const char* filename)
{
  std::vector<std::string> lines;
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in) {
    for (std::string line; std::getline(in, line);) {
      lines.emplace_back(line);
    }
  }
  return lines;
}

/**
 * @brief Reads the file with the given filename into byte array.
 * @param filename The path of the file to read from.
 * @return The contents of the file or an empty vector in case the content could
 * not be read.
 */
inline ArrayBuffer readBinaryFile(const char* filename)
{
  ArrayBuffer contents;
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in) {
    // Stop eating new lines in binary mode
    in.unsetf(std::ios::skipws);
    // Get the file size
    std::streampos fileSize;
    in.seekg(0, std::ios::end);
    fileSize = in.tellg();
    in.seekg(0, std::ios::beg);
    // Reserve capacity
    contents.reserve(static_cast<size_t>(fileSize));
    // Read the data:
    contents.insert(contents.begin(), std::istream_iterator<uint8_t>(in),
                    std::istream_iterator<uint8_t>());
  }
  return contents;
}

/**
 * @brief Removes the extension from a filename.
 * @param filename. The filename.
 * @return The filename with the extension removes.
 */
inline std::string removeExtension(const std::string& filename)
{
  auto p = filename.find_last_of('.');
  return p > 0 && p != std::string::npos ? filename.substr(0, p) : filename;
}

/**
 * @brief Standardizes a path: use forward slashes only, end with slash.
 * @param path The path to standardize.
 * @return The standardized path.
 */
inline std::string standardizePath(const std::string& path)
{
  std::string _path = path;
  std::replace(_path.begin(), _path.end(), '\\', '/');
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
inline bool writeFileContents(const char* filename, const std::string& contents)
{
  bool writtentoFile = false;
  std::ofstream out(filename, std::ios::out);
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
                           const std::vector<std::string>& lines)
{
  bool writtentoFile = false;
  std::ofstream out(filename, std::ios::out);
  if (out) {
    for (const auto& line : lines) {
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
