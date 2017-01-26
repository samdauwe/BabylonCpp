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
inline std::string extension(const std::string& filename)
{
  size_t pos = filename.find_last_of(".");
  if (pos == std::string::npos) {
    return "";
  }
  return filename.substr(pos + 1);
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
