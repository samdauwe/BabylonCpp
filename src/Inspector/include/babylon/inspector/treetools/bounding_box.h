#ifndef BABYLON_INSPECTOR_TREE_TOOLS_BOUNDING_BOX_H
#define BABYLON_INSPECTOR_TREE_TOOLS_BOUNDING_BOX_H

#include <babylon/imgui/icons_font_awesome.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

namespace BABYLON {

struct IToolBoundingBox;

/**
 * @brief Checkbox to display/hide the primitive.
 */
class BoundingBoxInsp : public AbstractTreeTool {

public:
  /** Font Awesome Icon Unicodes **/
  // fa-check-square-o
  static constexpr const char* VISIBLE_ICON = ICON_FA_CHECK_SQUARE_O;
  // fa-square-o
  static constexpr const char* INVISIBLE_ICON = ICON_FA_SQUARE_O;

public:
  BoundingBoxInsp(IToolBoundingBox* obj);
  virtual ~BoundingBoxInsp() override;

  BoundingBoxInsp(const BoundingBoxInsp& other) = delete;
  BoundingBoxInsp(BoundingBoxInsp&& other);

  void render() override;

protected:
  void action() override;

private:
  void _check();

private:
  IToolBoundingBox* _obj;

}; // end of class BoundingBoxInsp

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_BOUNDING_BOX_H
