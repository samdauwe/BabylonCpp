#include <babylon/loading/glTF/2.0/extensions/khr_materials_pbr_specular_glossiness.h>

#include <babylon/loading/glTF/2.0/extensions/ikhr_materials_pbr_specular_glossiness.h>
#include <babylon/loading/glTF/2.0/gltf_loader.h>
#include <babylon/loading/glTF/2.0/gltf_loader_interfaces.h>
#include <babylon/materials/material.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {
namespace GLTF2 {

const char* KHRMaterialsPbrSpecularGlossiness::name() const
{
  return "KHR_materials_pbrSpecularGlossiness";
}

bool KHRMaterialsPbrSpecularGlossiness::_loadMaterial(
  const GLTFLoader& loader, const string_t& context,
  const IGLTFMaterial& material,
  const ::std::function<void(Material* babylonMaterial, bool isNew)>& assign)
{
}

void KHRMaterialsPbrSpecularGlossiness::_loadSpecularGlossinessProperties(
  GLTFLoader& loader, const string_t& context, IGLTFMaterial& material,
  const IKHRMaterialsPbrSpecularGlossiness& properties)
{
  auto babylonMaterial = static_cast<PBRMaterial*>(material.babylonMaterial);

  babylonMaterial->albedoColor = !properties.diffuseFactor.empty() ?
                                   Color3::FromArray(properties.diffuseFactor) :
                                   Color3(1.f, 1.f, 1.f);
  babylonMaterial->reflectivityColor
    = !properties.specularFactor.empty() ?
        Color3::FromArray(properties.specularFactor) :
        Color3(1.f, 1.f, 1.f);
  babylonMaterial->microSurface
    = properties.glossinessFactor.isNull() ? 1.f : *properties.glossinessFactor;

  if (properties.diffuseTexture) {
    const auto index
      = properties.diffuseTexture ? (*properties.diffuseTexture).index : 0;
    if (index >= loader._gltf->textures.size()) {
      throw ::std::runtime_error(context + ": Failed to find diffuse texture "
                                 + ::std::to_string(index));
    }
    auto& texture = loader._gltf->textures[index];

    babylonMaterial->albedoTexture
      = loader._loadTexture("textures[" + ::std::to_string(texture.index) + "]",
                            texture, (*properties.diffuseTexture).texCoord);
  }

  if (properties.specularGlossinessTexture) {
    const auto index = properties.specularGlossinessTexture ?
                         (*properties.specularGlossinessTexture).index :
                         0;
    if (index >= loader._gltf->textures.size()) {
      throw ::std::runtime_error(context + ": Failed to find diffuse texture "
                                 + ::std::to_string(index));
    }
    auto& texture = loader._gltf->textures[index];

    babylonMaterial->reflectivityTexture = loader._loadTexture(
      "textures[" + ::std::to_string(texture.index) + "]", texture,
      (*properties.specularGlossinessTexture).texCoord);
    babylonMaterial->reflectivityTexture->setHasAlpha(true);
    babylonMaterial->useMicroSurfaceFromReflectivityMapAlpha = true;
  }

  loader._loadMaterialAlphaProperties(context, material,
                                      properties.diffuseFactor);
}

} // end of namespace GLTF2
} // end of namespace BABYLON
