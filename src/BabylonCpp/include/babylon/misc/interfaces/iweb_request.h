#ifndef BABYLON_MISC_INTERFACES_IWEB_REQUEST_H
#define BABYLON_MISC_INTERFACES_IWEB_REQUEST_H

#include <string>

namespace BABYLON {

/**
 * @brief Interface used to define the mechanism to get data from the network.
 */
struct IWebRequest {
  /**
   * Returns client's response url
   */
  std::string responseURL;
  /**
   * Returns client's status
   */
  std::string status;
  /**
   * Returns client's status as a text
   */
  std::string statusText;
}; // end of struct IWebRequest

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_INTERFACES_IWEB_REQUEST_H
