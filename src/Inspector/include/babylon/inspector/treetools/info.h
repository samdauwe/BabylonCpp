#ifndef BABYLON_INSPECTOR_TREE_TOOLS_INFO_H
#define BABYLON_INSPECTOR_TREE_TOOLS_INFO_H

#include <babylon/imgui/icons_font_awesome.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

namespace BABYLON {

struct IToolInfo;

/**
 * @brief Info panel to show information.
 */
class Info : public AbstractTreeTool {

public:
  /** Font Awesome Icon Unicodes **/
  // fa-info-circle
  static constexpr const char* VISIBLE_ICON = ICON_FA_INFO_CIRCLE;
  // fa-info-circle
  static constexpr const char* INVISIBLE_ICON = ICON_FA_INFO_CIRCLE;

public:
  Info(IToolInfo* obj);
  virtual ~Info() override;

  Info(const Info& other) = delete;
  Info(Info&& other);

  void render() override;

protected:
  void action() override;

private:
  IToolInfo* _obj;

}; // end of class Info

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_INFO_H
