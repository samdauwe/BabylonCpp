#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_CONE_EMITTER_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_CONE_EMITTER_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/particles/emittertypes/cone_particle_emitter.h>

namespace BABYLON {

FWD_CLASS_SPTR(ConeParticleEmitter)

struct BABYLON_SHARED_EXPORT ConeEmitterGridComponent {

  static void render(const ConeParticleEmitterPtr& emitter)
  {
    auto sliderChange
      = SliderLineComponent::render("Radius range", emitter->radiusRange, 0.f, 1.f, 0.01f, "%.2f");
    if (sliderChange) {
      emitter->radiusRange = sliderChange.value();
    }
    sliderChange
      = SliderLineComponent::render("Height range", emitter->heightRange, 0.f, 1.f, 0.01f, "%.2f");
    if (sliderChange) {
      emitter->heightRange = sliderChange.value();
    }
    if (CheckBoxLineComponent::render("Emit from spawn point only",
                                      emitter->emitFromSpawnPointOnly)) {
      emitter->emitFromSpawnPointOnly = !emitter->emitFromSpawnPointOnly;
    }
    sliderChange = SliderLineComponent::render("Direction randomizer", emitter->directionRandomizer,
                                               0.f, 1.f, 0.01f, "%.2f");
    if (sliderChange) {
      emitter->directionRandomizer = sliderChange.value();
    }
  }

}; // end of struct ConeEmitterGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_CONE_EMITTER_GRID_COMPONENT_H
