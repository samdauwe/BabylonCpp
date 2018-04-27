#ifndef BABYLON_INSPECTOR_TREE_TOOLS_CAMERA_POV_H
#define BABYLON_INSPECTOR_TREE_TOOLS_CAMERA_POV_H

#include <babylon/imgui/icons_font_awesome.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

namespace BABYLON {

struct ICameraPOV;

class CameraPOV : public AbstractTreeTool {

public:
  /** Font Awesome Icon Unicodes **/
  // fa-check-circle
  static constexpr const char* POV_ACTIVE_ICON = ICON_FA_CHECK_CIRCLE;
  // fa-circle
  static constexpr const char* POV_INACTIVE_ICON = ICON_FA_CIRCLE;

public:
  CameraPOV(ICameraPOV* camera);
  virtual ~CameraPOV() override;

  CameraPOV(const CameraPOV& other) = delete;
  CameraPOV(CameraPOV&& other);

  void render() override;

protected:
  void action() override;

private:
  void _gotoPOV();

private:
  ICameraPOV* _cameraPOV;

}; // end of class CameraPOV

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_CAMERA_POV_H
