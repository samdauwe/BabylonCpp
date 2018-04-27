#ifndef BABYLON_INSPECTOR_TREE_TOOLS_CHECKBOX_H
#define BABYLON_INSPECTOR_TREE_TOOLS_CHECKBOX_H

#include <babylon/imgui/icons_font_awesome.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

namespace BABYLON {

struct IToolVisible;

/**
 * @brief Checkbox to display/hide the primitive.
 */
class Checkbox : public AbstractTreeTool {

public:
  /** Font Awesome Icon Unicodes **/
  // fa-eye
  static constexpr const char* VISIBLE_ICON = ICON_FA_EYE;
  // fa-eye-slash
  static constexpr const char* INVISIBLE_ICON = ICON_FA_EYE_SLASH;

public:
  Checkbox(IToolVisible* obj);
  virtual ~Checkbox() override;

  Checkbox(const Checkbox& other) = delete;
  Checkbox(Checkbox&& other);

  void render() override;

protected:
  void action() override;

private:
  void _check(bool dontEnable = false);

private:
  IToolVisible* _obj;

}; // end of class Checkbox

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_CHECKBOX_H
