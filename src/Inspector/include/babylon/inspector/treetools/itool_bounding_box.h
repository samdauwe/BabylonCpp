#ifndef BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_BOUNDING_BOX_H
#define BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_BOUNDING_BOX_H

namespace BABYLON {

/**
 * @brief Any object implementing this interface should provide methods to
 * toggle its bounding box.
 */
struct IToolBoundingBox {

  virtual bool isBoxVisible() const = 0;

  virtual void setBoxVisible(bool b) = 0;

}; // end of struct IToolBoundingBox

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_ITOOL_BOUNDING_BOX_H
