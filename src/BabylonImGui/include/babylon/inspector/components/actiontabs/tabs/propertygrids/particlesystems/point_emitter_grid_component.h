#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_POINT_EMITTER_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_POINT_EMITTER_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/particles/emittertypes/point_particle_emitter.h>

namespace BABYLON {

FWD_CLASS_SPTR(PointParticleEmitter)

struct BABYLON_SHARED_EXPORT PointEmitterGridComponent {

  static void render(const PointParticleEmitterPtr& emitter)
  {
    Vector3LineComponent::render("Direction 1", emitter->direction1);
    Vector3LineComponent::render("Direction 2", emitter->direction2);
  }

}; // end of struct PointEmitterGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_POINT_EMITTER_GRID_COMPONENT_H
