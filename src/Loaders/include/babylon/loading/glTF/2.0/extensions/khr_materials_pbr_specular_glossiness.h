#ifndef BABYLON_LOADING_GLTF_2_0_EXTENSIONS_KHR_MATERIALS_PBR_SPECULAR_GLOSSINESSS_H
#define BABYLON_LOADING_GLTF_2_0_EXTENSIONS_KHR_MATERIALS_PBR_SPECULAR_GLOSSINESSS_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace GLTF2 {

class GLTFLoader;
struct IGLTFMaterial;
struct IKHRMaterialsPbrSpecularGlossiness;

class BABYLON_SHARED_EXPORT KHRMaterialsPbrSpecularGlossiness
  : public GLTFLoaderExtension {

public:
  const char* name() const;

protected:
  bool _loadMaterial(
    const GLTFLoader& loader, const string_t& context,
    const IGLTFMaterial& material,
    const std::function<void(Material* babylonMaterial, bool isNew)>& assign);

private:
  void _loadSpecularGlossinessProperties(
    GLTFLoader& loader, const string_t& context, IGLTFMaterial& material,
    const IKHRMaterialsPbrSpecularGlossiness& properties);

}; // end of class KHRMaterialsPbrSpecularGlossiness

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of
// BABYLON_LOADING_GLTF_2_0_EXTENSIONS_KHR_MATERIALS_PBR_SPECULAR_GLOSSINESSS_H
