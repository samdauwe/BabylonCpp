#ifndef BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_EXTENSION_H
#define BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_EXTENSION_H

#include <babylon/babylon_api.h>
#include <babylon/loading/glTF/igltf_loader.h>

namespace BABYLON {

class BaseTexture;
class Camera;
class Geometry;
class Material;
class Mesh;
class TransformNode;
using BaseTexturePtr   = std::shared_ptr<BaseTexture>;
using CameraPtr        = std::shared_ptr<Camera>;
using GeometryPtr      = std::shared_ptr<Geometry>;
using MaterialPtr      = std::shared_ptr<Material>;
using MeshPtr          = std::shared_ptr<Mesh>;
using TransformNodePtr = std::shared_ptr<TransformNode>;

namespace GLTF2 {

struct IAnimation;
struct ICamera;
struct IMaterial;
struct IMesh;
struct IMeshPrimitive;
struct INode;
struct IProperty;
struct ISkin;
struct IScene;
struct ITextureInfo;

/**
 * @brief Interface for a glTF loader extension.
 */
struct BABYLON_SHARED_EXPORT IGLTFLoaderExtension : public IGLTFBaseLoaderExtension,
                                                    public IDisposable {
  /**
   * @brief Called after the loader state changes to LOADING.
   */
  virtual void onLoading();

  /**
   * @brief Called after the loader state changes to READY.
   */
  virtual void onReady();

  /**
   * @brief Define this method to modify the default behavior when loading scenes.
   * @param context The context when loading the asset
   * @param scene The glTF scene property
   * @returns A promise that resolves when the load is complete or null if not handled
   */
  virtual void loadSceneAsync(const std::string& context, const IScene& scene);

  /**
   * @brief Define this method to modify the default behavior when loading nodes.
   * @param context The context when loading the asset
   * @param node The glTF node property
   * @param assign A function called synchronously after parsing the glTF properties
   * @returns A promise that resolves with the loaded Babylon transform node when the load is
   * complete or null if not handled
   */
  virtual TransformNodePtr
  loadNodeAsync(const std::string& context, const INode& node,
                const std::function<void(const TransformNode& babylonMesh)>& assign);

  /**
   * @brief Define this method to modify the default behavior when loading cameras.
   * @param context The context when loading the asset
   * @param camera The glTF camera property
   * @param assign A function called synchronously after parsing the glTF properties
   * @returns A promise that resolves with the loaded Babylon camera when the load is complete or
   * null if not handled
   */
  virtual CameraPtr loadCameraAsync(const std::string& context, const ICamera& camera,
                                    const std::function<void(const Camera& babylonCamera)>& assign);

  /**
   * @brief  Define this method to modify the default behavior when loading vertex data for mesh
   * primitives.
   * @param context The context when loading the asset
   * @param primitive The glTF mesh primitive property
   * @returns A promise that resolves with the loaded geometry when the load is complete or null if
   * not handled
   */
  virtual GeometryPtr _loadVertexDataAsync(const std::string& context,
                                           const IMeshPrimitive& primitive,
                                           const MeshPtr& babylonMesh);

  /**
   * @brief Define this method to modify the default behavior when loading data for mesh primitives.
   * @param context The context when loading the asset
   * @param name The mesh name when loading the asset
   * @param node The glTF node when loading the asset
   * @param mesh The glTF mesh when loading the asset
   * @param primitive The glTF mesh primitive property
   * @param assign A function called synchronously after parsing the glTF properties
   * @returns A promise that resolves with the loaded mesh when the load is complete or null if not
   * handled
   */
  virtual AbstractMeshPtr
  _loadMeshPrimitiveAsync(const std::string& context, const std::string& name, const INode& node,
                          const IMesh& mesh, const IMeshPrimitive& primitive,
                          const std::function<void(const AbstractMeshPtr& babylonMesh)>& assign);

  /**
   * @brief  Define this method to modify the default behavior when loading materials. Load material
   * creates the material and then loads material properties.
   * @param context The context when loading the asset
   * @param material The glTF material property
   * @param assign A function called synchronously after parsing the glTF
   * properties
   * @returns A promise that resolves with the loaded Babylon material when the load is complete or
   * null if not handled
   */
  virtual MaterialPtr
  _loadMaterialAsync(const std::string& context, const IMaterial& material,
                     const MeshPtr& babylonMesh, unsigned int babylonDrawMode,
                     const std::function<void(const MaterialPtr& babylonMaterial)>& assign);

  /**
   * @brief Define this method to modify the default behavior when creating materials.
   * @param context The context when loading the asset
   * @param material The glTF material property
   * @param babylonDrawMode The draw mode for the Babylon material
   * @returns The Babylon material or null if not handled
   */
  virtual MaterialPtr createMaterial(const std::string& context, const IMaterial& material,
                                     unsigned int babylonDrawMode);

  /**
   * @brief Define this method to modify the default behavior when loading material properties.
   * @param context The context when loading the asset
   * @param material The glTF material property
   * @param babylonMaterial The Babylon material
   * @returns A promise that resolves when the load is complete or null if not handled
   */
  virtual void loadMaterialPropertiesAsync(const std::string& context, const IMaterial& material,
                                           const Material& babylonMaterial);

  /**
   * @brief Define this method to modify the default behavior when loading texture infos.
   * @param context The context when loading the asset
   * @param textureInfo The glTF texture info property
   * @param assign A function called synchronously after parsing the glTF
   * properties
   * @returns A promise that resolves with the loaded Babylon texture when the load is complete or
   * null if not handled
   */
  virtual BaseTexturePtr
  loadTextureInfoAsync(const std::string& context, const ITextureInfo& textureInfo,
                       const std::function<void(const BaseTexture& babylonTexture)>& assign);

  /**
   * @brief Define this method to modify the default behavior when loading animations.
   * @param context The context when loading the asset
   * @param animation The glTF animation property
   * @returns A promise that resolves with the loaded Babylon animation group when the load is
   * complete or null if not handled
   */
  virtual AnimationGroupPtr loadAnimationAsync(const std::string& context,
                                               const IAnimation& animation);

  /**
   * @brief Define this method to modify the default behavior when loading skins.
   * @param context The context when loading the asset
   * @param node The glTF node property
   * @param skin The glTF skin property
   * @returns A promise that resolves when the load is complete or null if not handled
   */
  virtual void _loadSkinAsync(const std::string& context, const INode& node, const ISkin& skin);

  /**
   * @brief Define this method to modify the default behavior when loading uris.
   * @param context The context when loading the asset
   * @param uri The uri to load
   * @returns A promise that resolves with the loaded data when the load is complete or null if not
   * handled
   */
  virtual ArrayBufferView _loadUriAsync(const std::string& context, const IProperty& property,
                                        const std::string& uri);

}; // end of struct IGLTFLoaderExtension

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_EXTENSION_H
