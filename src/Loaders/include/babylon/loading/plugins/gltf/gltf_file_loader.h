#ifndef BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_H
#define BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_H

#include <babylon/babylon_global.h>
#include <babylon/loading/iscene_loader_plugin_async.h>
#include <babylon/loading/plugins/gltf/gltf_file_loader_enums.h>
#include <babylon/loading/plugins/gltf/gltf_file_loader_interfaces.h>

namespace BABYLON {

class Tokenizer;

/**
 * @brief glTF File Loader Plugin.
 */
class BABYLON_SHARED_EXPORT GLTFFileLoader : public ISceneLoaderPluginAsync {

public:
  /**
   * Static members
   */
  static const bool MakeYUP;
  static const bool HomogeneousCoordinates;

  static const std::vector<std::string> glTFTransforms;
  static const std::vector<std::string> babylonTransforms;

  static const std::vector<std::string> glTFAnimationPaths;
  static const std::vector<std::string> babylonAnimationPaths;

public:
  GLTFFileLoader();
  virtual ~GLTFFileLoader();

  /**
   * Import meshes
   */
  bool importMeshAsync(
    const std::vector<std::string>& meshesNames, Scene* scene,
    const std::string& data, const std::string& rootUrl,
    const std::function<void(std::vector<AbstractMesh*>& meshes,
                             std::vector<ParticleSystem*>& particleSystems,
                             std::vector<Skeleton*>& skeletons)>& onSuccess,
    const std::function<void()>& onError) override;
  bool loadAsync(Scene* scene, const std::string& data,
                 const std::string& rootUrl,
                 const std::function<void()>& onsuccess,
                 const std::function<void()>& onerror) override;

  /**
   * Parse
   */
  static void parseBuffers(const Json::object& parsedBuffers,
                           IGLTFRuntime& gltfRuntime);
  static void parseShaders(const Json::object& parsedShaders,
                           IGLTFRuntime& gltfRuntime);
  static void parseObject(const Json::object& parsedObjects,
                          const std::string& runtimeProperty,
                          IGLTFRuntime& gltfRuntime);

  /**
   * Utils
   */
  void normalizeUVs(Float32Array& buffer) const;
  std::string replaceInString(const std::string& str,
                              const std::string& searchValue,
                              const std::string& replaceValue) const;
  std::string
  getAttribute(const IGLTFTechniqueParameter& attributeParameter) const;

  /**
   * Returns the animation path (glTF -> Babylon)
   */
  std::string getAnimationPath(const std::string& path) const;

  /**
   * Loads and creates animations
   */
  void loadAnimations(const IGLTFRuntime& gltfRuntime) const;

  /**
   * Returns the bones transformation matrix
   */
  Matrix configureBoneTransformation(const IGLTFNode& node) const;

  /**
   * Returns the parent bone
   */
  Bone* getParentBone(const IGLTFRuntime& gltfRuntime, const IGLTFSkins& skins,
                      const std::string& jointName,
                      Skeleton* newSkeleton) const;

  /**
   * Returns the appropriate root node
   */
  Bone* getNodeToRoot(const std::vector<INodeToRoot> nodesToRoot,
                      const std::string& id) const;

  /**
   * Returns the node with the joint name
   */
  IJointNode getJointNode(const IGLTFRuntime& gltfRuntime,
                          const std::string& jointName) const;

  /**
   * Checks if a nodes is in joints
   */
  bool nodeIsInJoints(const IGLTFSkins& skins, const std::string& id) const;

  /**
   * Fills the nodes to root for bones and builds hierarchy
   */
  void getNodesToRoot(const IGLTFRuntime& gltfRuntime, Skeleton* newSkeleton,
                      const IGLTFSkins& skins,
                      std::vector<INodeToRoot>& nodesToRoot) const;

  void printMat(const Float32Array& m) const;

  /**
   * Imports a skeleton
   */
  Skeleton* importSkeleton(const IGLTFRuntime& gltfRuntime,
                           const IGLTFSkins& skins, Mesh* mesh,
                           Skeleton* newSkeleton) const;

  /**
   * Imports a mesh and its geometries
   */
  Mesh* importMesh(const IGLTFRuntime& gltfRuntime, const IGLTFNode& node,
                   std::vector<std::string> meshes, const std::string& id,
                   Mesh* newMesh) const;

  /**
   * Configure node transformation from position, rotation and scaling
   */
  void configureNode(Mesh* newNode, const Vector3& position,
                     const Quaternion& rotation, const Vector3& scaling) const;

  /**
   * Configures node from transformation matrix
   */
  void configureNodeFromMatrix(Mesh* newNode, const IGLTFNode& node,
                               Node* parent) const;

  /**
   * Imports a node
   */
  Node* importNode(const IGLTFRuntime& gltfRuntime, const IGLTFNode& node,
                   const std::string& id) const;

  /**
   * Traverses nodes and creates them
   */
  void traverseNodes(const IGLTFRuntime& gltfRuntime, const std::string& id,
                     Node* parent, bool meshIncluded = false) const;

  /**
   * do stuff after buffers, shaders are loaded (e.g. hook up materials, load
   * animations, etc.)
   */
  void postLoad(const IGLTFRuntime& gltfRuntime) const;

  /**
   * onBind shaderrs callback to set uniforms and matrices
   */
  void onBindShaderMaterial(Mesh* mesh, const IGLTFRuntime& gltfRuntime,
                            const std::string& unTreatedUniforms,
                            ShaderMaterial* shaderMaterial,
                            const IGLTFTechnique& technique,
                            const IGLTFMaterial& material) const;

  /**
   * Prepare uniforms to send the only one time
   * Loads the appropriate textures
   */
  void prepareShaderMaterialUniforms(
    const IGLTFRuntime& gltfRuntime, ShaderMaterial* shaderMaterial,
    const IGLTFTechnique& technique, const IGLTFMaterial& material,
    const std::string& unTreatedUniforms) const;

  /**
   * Shader compilation failed
   */
  void onShaderCompileError(const IGLTFProgram& program,
                            ShaderMaterial* shaderMaterial) const;

  /**
   * Shader compilation success
   */
  void onShaderCompileSuccess(const IGLTFRuntime& gltfRuntime,
                              ShaderMaterial* shaderMaterial,
                              const IGLTFTechnique& technique,
                              const IGLTFMaterial& material,
                              const std::string& unTreatedUniforms) const;

  /**
   * Returns the appropriate uniform if already handled by babylon
   */
  std::string parseShaderUniforms(const Tokenizer& tokenizer,
                                  const IGLTFTechnique& technique,
                                  const std::string& unTreatedUniforms) const;

  /**
   * All shaders loaded. Create materials one by one
   */
  void importMaterials(const IGLTFRuntime& gltfRuntime);

private:
  void _loadShadersAsync(const IGLTFRuntime& gltfRuntime,
                         const std::function<void()>& onload);
  void _loadBuffersAsync(const IGLTFRuntime& gltfRuntime,
                         const std::function<void()>& onload);
  // Creates nodes before loading buffers and shaders
  void _createNodes(const IGLTFRuntime& gltfRuntime) const;

public:
  /**
   * Public members
   */
  std::string extensions = ".gltf";

}; // end of class GLTFFileLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_H
