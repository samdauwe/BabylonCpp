#ifndef BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_INFO_H
#define BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_INFO_H

namespace BABYLON {

/**
 * @brief Any object implementing this interface should provide methods to
 * retrieve its info.
 */
struct IToolInfo {

  virtual string_t getInfo() const = 0;

}; // end of struct IToolInfo

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_INFO_H
