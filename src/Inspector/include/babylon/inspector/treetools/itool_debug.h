#ifndef BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_DEBUG_H
#define BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_DEBUG_H

namespace BABYLON {

/**
 * @brief Any object implementing this interface should provide methods to
 * toggle a debug area.
 */
struct IToolDebug {

  virtual void debug(bool b) const = 0;

}; // end of struct IToolDebug

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_DEBUG_H
