#ifndef BABYLON_INSPECTOR_TREE_TOOLS_DEBUG_AREA_H
#define BABYLON_INSPECTOR_TREE_TOOLS_DEBUG_AREA_H

#include <babylon/imgui/icons_font_awesome.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

namespace BABYLON {

struct IToolDebug;

/**
 * @brief Debug area class.
 */
class DebugArea : public AbstractTreeTool {

public:
  /** Font Awesome Icon Unicodes **/
  // fa-wrench
  static constexpr const char* VISIBLE_ICON = ICON_FA_WRENCH;
  // fa-wrench
  static constexpr const char* INVISIBLE_ICON = ICON_FA_WRENCH;

public:
  DebugArea(IToolDebug* obj);
  virtual ~DebugArea() override;

  DebugArea(const DebugArea& other) = delete;
  DebugArea(DebugArea&& other);

  void render() override;

protected:
  void action() override;

private:
  IToolDebug* _obj;

}; // end of class DebugArea

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_DEBUG_AREA_H
