#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_MESH_EMITTER_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_MESH_EMITTER_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/particles/emittertypes/mesh_particle_emitter.h>

namespace BABYLON {

FWD_CLASS_SPTR(MeshParticleEmitter)

struct BABYLON_SHARED_EXPORT MeshEmitterGridComponent {

  static void render(const MeshParticleEmitterPtr& emitter)
  {
    if (!emitter->useMeshNormalsForDirection) {
      Vector3LineComponent::render("Direction 1", emitter->direction1);
      Vector3LineComponent::render("Direction 2", emitter->direction2);
    }
    if (CheckBoxLineComponent::render("Use normals for direction",
                                      emitter->useMeshNormalsForDirection)) {
      emitter->useMeshNormalsForDirection = !emitter->useMeshNormalsForDirection;
    }
  }

}; // end of struct MeshEmitterGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_PARTICLE_SYSTEMS_MESH_EMITTER_GRID_COMPONENT_H
