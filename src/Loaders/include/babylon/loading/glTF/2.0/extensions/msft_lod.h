#ifndef BABYLON_LOADING_GLTF_2_0_EXTENSIONS_MSFT_LOD_H
#define BABYLON_LOADING_GLTF_2_0_EXTENSIONS_MSFT_LOD_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace GLTF2 {

/**
 * @brief See
 * https://github.com/sbtron/glTF/tree/MSFT_lod/extensions/Vendor/MSFT_lod for
 * more information about this extension.
 */
class BABYLON_SHARED_EXPORT MSFTLOD : public GLTFLoaderExtension {

public:
  const char* name() const;

protected:
  bool _traverseNode(
    const GLTFLoader& loader, const string_t& context, IGLTFNode* node:,
    const ::std::function<bool(IGLTFNode* node, IGLTFNode* node)> action,
    IGLTFNode* parentNode);

  bool _loadNode(const GLTFLoader& loader, const string_t& context,
                 IGLTFNode* node);

  bool _loadMaterial(
    const GLTFLoader& loader, const string_t& context,
    const IGLTFMaterial& material,
    const ::std::function<void(Material* babylonMaterial, bool isNew)>& assign);

private:
  void _loadNodeLOD(const GLTFLoader& loader, const string_t& context,
                    const vector_t<IGLTFNode*> nodes, unsigned int index,
                    ::std::function<void()>& onComplete);

  void _loadMaterialLOD(
    const GLTFLoader& loader, const string_t& context,
    const vector_t<IGLTFMaterial*>& materials, unsigned int index,
    const ::std::function<void(Material* babylonMaterial, bool isNew)>& assign,
    const ::std::function<void()>& onComplete);

}; // end of class KHRMaterialsPbrSpecularGlossiness

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of
// BABYLON_LOADING_GLTF_2_0_EXTENSIONS_MSFT_LOD_H
