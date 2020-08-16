#include <babylon/materials/thin_material_helper.h>

#include <babylon/materials/effect.h>
#include <babylon/misc/interfaces/iclip_planes_holder.h>

namespace BABYLON {

void ThinMaterialHelper::BindClipPlane(const EffectPtr& effect, const IClipPlanesHolder& holder)
{
  if (holder.clipPlane) {
    const auto& clipPlane = *holder.clipPlane;
    effect->setFloat4("vClipPlane", clipPlane.normal.x, clipPlane.normal.y, clipPlane.normal.z,
                      clipPlane.d);
  }
  if (holder.clipPlane2) {
    const auto& clipPlane = *holder.clipPlane2;
    effect->setFloat4("vClipPlane2", clipPlane.normal.x, clipPlane.normal.y, clipPlane.normal.z,
                      clipPlane.d);
  }
  if (holder.clipPlane3) {
    const auto& clipPlane = *holder.clipPlane3;
    effect->setFloat4("vClipPlane3", clipPlane.normal.x, clipPlane.normal.y, clipPlane.normal.z,
                      clipPlane.d);
  }
  if (holder.clipPlane4) {
    const auto& clipPlane = *holder.clipPlane4;
    effect->setFloat4("vClipPlane4", clipPlane.normal.x, clipPlane.normal.y, clipPlane.normal.z,
                      clipPlane.d);
  }
  if (holder.clipPlane5) {
    const auto& clipPlane = *holder.clipPlane5;
    effect->setFloat4("vClipPlane5", clipPlane.normal.x, clipPlane.normal.y, clipPlane.normal.z,
                      clipPlane.d);
  }
  if (holder.clipPlane6) {
    const auto& clipPlane = *holder.clipPlane6;
    effect->setFloat4("vClipPlane6", clipPlane.normal.x, clipPlane.normal.y, clipPlane.normal.z,
                      clipPlane.d);
  }
}

} // end of namespace BABYLON
