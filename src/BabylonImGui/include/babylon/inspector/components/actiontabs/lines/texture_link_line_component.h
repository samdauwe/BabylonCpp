#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_TEXTURE_LINK_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_TEXTURE_LINK_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/materials/material.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TextureLinkLineComponent {

  static void render(const char* /*label*/, const BaseTexturePtr& /*texture*/)
  {
  }

  static void render(const char* /*label*/, const MaterialPtr& /*material*/,
                     const BaseTexturePtr& /*texture*/)
  {
  }

}; // end of struct TextureLinkLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_TEXTURE_LINK_LINE_COMPONENT_H
