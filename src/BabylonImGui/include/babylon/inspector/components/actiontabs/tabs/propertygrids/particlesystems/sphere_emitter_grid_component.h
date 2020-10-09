#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_SPHERE_EMITTER_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_SPHERE_EMITTER_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/particles/emittertypes/sphere_particle_emitter.h>

namespace BABYLON {

FWD_CLASS_SPTR(SphereParticleEmitter)

struct BABYLON_SHARED_EXPORT SphereEmitterGridComponent {

  static void render(const SphereParticleEmitterPtr& emitter)
  {
    auto valueChange = FloatLineComponent::render("Radius", emitter->radius);
    if (valueChange) {
      emitter->radius = valueChange.value();
    }
    auto sliderChange
      = SliderLineComponent::render("Radius range", emitter->radiusRange, 0.f, 1.f, 0.01f, "%.2f");
    if (sliderChange) {
      emitter->radiusRange = sliderChange.value();
    }
    sliderChange = SliderLineComponent::render("Direction randomizer", emitter->directionRandomizer,
                                               0.f, 1.f, 0.01f, "%.2f");
    if (sliderChange) {
      emitter->directionRandomizer = sliderChange.value();
    }
  }

}; // end of struct SphereEmitterGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_SPHERE_EMITTER_GRID_COMPONENT_H
