#ifndef BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_INFO_H
#define BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_INFO_H

#include <string>

namespace BABYLON {

/**
 * @brief Any object implementing this interface should provide methods to
 * retrieve its info.
 */
struct IToolInfo {

  virtual std::string getInfo() const = 0;

}; // end of struct IToolInfo

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_INFO_H
