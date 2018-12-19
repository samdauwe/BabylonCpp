#ifndef BABYLON_LOADING_IFILE_INFO_H
#define BABYLON_LOADING_IFILE_INFO_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines file information
 */
struct BABYLON_SHARED_EXPORT IFileInfo {
  /**
   * Gets the file url
   */
  std::string url;
  /**
   * Gets the root url
   */
  std::string rootUrl;
  /**
   * Gets filename
   */
  std::string name;
}; // end of struct IFileInfo

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_IFILE_INFO_H
