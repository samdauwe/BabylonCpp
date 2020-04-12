#include <babylon/loading/plugins/gltf/2.0/gltf_loader_extension.h>

#include <babylon/core/array_buffer_view.h>

namespace BABYLON {
namespace GLTF2 {

void IGLTFLoaderExtension::onLoading()
{
}

void IGLTFLoaderExtension::onReady()
{
}

void IGLTFLoaderExtension::loadSceneAsync(const std::string& /*context*/, const IScene& /*scene*/)
{
}

TransformNodePtr IGLTFLoaderExtension::loadNodeAsync(
  const std::string& /*context*/, const INode& /*node*/,
  const std::function<void(const TransformNode& babylonMesh)>& /*assign*/)
{
  return nullptr;
}

CameraPtr IGLTFLoaderExtension::loadCameraAsync(
  const std::string& /*context*/, const ICamera& /*camera*/,
  const std::function<void(const Camera& babylonCamera)>& /*assign*/)
{
  return nullptr;
}

GeometryPtr IGLTFLoaderExtension::_loadVertexDataAsync(const std::string& /*context*/,
                                                       const IMeshPrimitive& /*primitive*/,
                                                       const MeshPtr& /*babylonMesh*/)
{
  return nullptr;
}

AbstractMeshPtr IGLTFLoaderExtension::_loadMeshPrimitiveAsync(
  const std::string& /*context*/, const std::string& /*name*/, const INode& /*node*/,
  const IMesh& /*mesh*/, const IMeshPrimitive& /*primitive*/,
  const std::function<void(const AbstractMeshPtr& babylonMesh)>& /*assign*/)
{
  return nullptr;
}

MaterialPtr IGLTFLoaderExtension::_loadMaterialAsync(
  const std::string& /*context*/, const IMaterial& /*material*/, const MeshPtr& /*babylonMesh*/,
  unsigned int /*babylonDrawMode*/,
  const std::function<void(const MaterialPtr& babylonMaterial)>& /*assign*/)
{
  return nullptr;
}

MaterialPtr IGLTFLoaderExtension::createMaterial(const std::string& /*context*/,
                                                 const IMaterial& /*material*/,
                                                 unsigned int /*babylonDrawMode*/)
{
  return nullptr;
}

void IGLTFLoaderExtension::loadMaterialPropertiesAsync(const std::string& /*context*/,
                                                       const IMaterial& /*material*/,
                                                       const Material& /*babylonMaterial*/)
{
}

BaseTexturePtr IGLTFLoaderExtension::loadTextureInfoAsync(
  const std::string& /*context*/, const ITextureInfo& /*textureInfo*/,
  const std::function<void(const BaseTexture& babylonTexture)>& /*assign*/)
{
  return nullptr;
}

AnimationGroupPtr IGLTFLoaderExtension::loadAnimationAsync(const std::string& /*context*/,
                                                           const IAnimation& /*animation*/)
{
  return nullptr;
}

void IGLTFLoaderExtension::_loadSkinAsync(const std::string& /*context*/, const INode& /*node*/,
                                          const ISkin& /*skin*/)
{
}

ArrayBufferView IGLTFLoaderExtension::_loadUriAsync(const std::string& /*context*/,
                                                    const IProperty& /*property*/,
                                                    const std::string& /*uri*/)
{
  return ArrayBufferView();
}

} // end of namespace GLTF2
} // end of namespace BABYLON
