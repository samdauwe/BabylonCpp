#ifndef BABYLON_CORE_SYTEM_H
#define BABYLON_CORE_SYTEM_H

#include <babylon/babylon_api.h>

#include <string>

namespace BABYLON {
namespace System {

  /**
   * @brief returns the current executable path
   */
  std::string BABYLON_SHARED_EXPORT getExecutablePath();

  /**
   * @brief change the current dir to the dir containing the current executable.
   */
  void BABYLON_SHARED_EXPORT chdirToExecutableFolder();


  /**
   * @brief opens an url with the  default browser
   */
  void BABYLON_SHARED_EXPORT openBrowser(const std::string &url);


  /**
   * @brief opens a file with the associated app
   */
  void BABYLON_SHARED_EXPORT openFile(const std::string &filename);


} // end of namespace System
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_SYTEM_H
