#ifndef BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_VISIBLE_H
#define BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_VISIBLE_H

namespace BABYLON {

/**
 * @brief Any object implementing this interface should  provide methods to
 * toggle its visibility.
 */
struct IToolVisible {

  virtual bool isVisible() const = 0;

  virtual void setVisible(bool b) = 0;

}; // end of struct IToolVisible

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_VISIBLE_H
