#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_PARTICLE_SYSTEM_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_PARTICLE_SYSTEM_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/inspector/components/actiontabs/lines/button_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/options_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/texture_link_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/particles/iparticle_system.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

FWD_CLASS_SPTR(IParticleSystem)

struct BABYLON_SHARED_EXPORT ParticleSystemPropertyGridComponent {

  static void render(const IParticleSystemPtr& system)
  {
    static std::vector<std::pair<const char*, unsigned int>> blendModeOptions{
      {"Add", ParticleSystem::BLENDMODE_ADD},
      {"Multiply", ParticleSystem::BLENDMODE_MULTIPLY},
      {"Multiply Add", ParticleSystem::BLENDMODE_MULTIPLYADD},
      {"OneOne", ParticleSystem::BLENDMODE_ONEONE},
      {"Standard", ParticleSystem::BLENDMODE_STANDARD},
    };

    static std::vector<std::pair<const char*, unsigned int>> particleEmitterTypeOptions{
      {"Box", 0},         //
      {"Cone", 1},        //
      {"Cylinder", 2},    //
      {"Hemispheric", 3}, //
      {"Mesh", 4},        //
      {"Point", 5},       //
      {"Sphere", 6},      //
    };

    static std::vector<std::pair<const char*, unsigned int>> emitterOptions{
      {"None", 100},  //
      {"Vector3", 0}, //
    };

    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("ID", system->id);
      TextLineComponent::render("Name", system->name);
      TextLineComponent::render("Class", system->getClassName());
      TextLineComponent::render("Capacity", std::to_string(system->getCapacity()));
      TextLineComponent::render("Active count", std::to_string(system->getActiveCount()));
      TextureLinkLineComponent::render("Texture", system->particleTexture);
      auto optionChange
        = OptionsLineComponent::render("Blend mode", system->blendMode, blendModeOptions);
      if (optionChange) {
        system->blendMode = optionChange.value();
      }
      Vector3LineComponent::render("World offset", system->worldOffset);
      Vector3LineComponent::render("Gravity", system->gravity);
      if (CheckBoxLineComponent::render("Is billboard", system->isBillboardBased)) {
        system->isBillboardBased = !system->isBillboardBased;
      }
      if (CheckBoxLineComponent::render("Is local", system->isLocal)) {
        system->isLocal = !system->isLocal;
      }
      if (CheckBoxLineComponent::render("Force depth write", system->forceDepthWrite)) {
        system->forceDepthWrite = !system->forceDepthWrite;
      }
      auto sliderChange = SliderLineComponent::render("Update speed", system->updateSpeed, 0.f,
                                                      0.1f, 0.001f, "%.3f");
      if (sliderChange) {
        system->updateSpeed = sliderChange.value();
      }
      generalContainerOpened = true;
    }
    else {
      generalContainerOpened = false;
    }
    // --- COMMANDS ---
    static auto commandsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(commandsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("COMMANDS")) {
      if (ButtonLineComponent::render("Dispose")) {
        system->dispose();
      }
      commandsContainerOpened = true;
    }
    else {
      commandsContainerOpened = false;
    }
  }

}; // end of struct ParticleSystemPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_PARTICLE_SYSTEM_PROPERTY_GRID_COMPONENT_H
