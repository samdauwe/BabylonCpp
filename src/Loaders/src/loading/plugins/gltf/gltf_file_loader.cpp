#include <babylon/loading/plugins/gltf/gltf_file_loader.h>

#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/node.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/matrix.h>
#include <babylon/mesh/mesh.h>
#include <babylon/utils/picojson.h>

namespace BABYLON {

const bool GLTFFileLoader::MakeYUP                = false;
const bool GLTFFileLoader::HomogeneousCoordinates = false;

const std::vector<std::string> GLTFFileLoader::glTFTransforms{
  "MODEL",      "VIEW", "PROJECTION", "MODELVIEW", "MODELVIEWPROJECTION",
  "JOINTMATRIX"};
const std::vector<std::string> GLTFFileLoader::babylonTransforms{
  "world", "view", "projection", "worldView", "worldViewProjection", "mBones"};

const std::vector<std::string> GLTFFileLoader::glTFAnimationPaths{
  "translation", "rotation", "scale"};
const std::vector<std::string> GLTFFileLoader::babylonAnimationPaths{
  "position", "rotationQuaternion", "scaling"};

GLTFFileLoader::GLTFFileLoader()
{
}

GLTFFileLoader::~GLTFFileLoader()
{
}

bool GLTFFileLoader::importMeshAsync(
  const std::vector<std::string>& meshesNames, Scene* scene,
  const std::string& data, const std::string& rootUrl,
  const std::function<void(std::vector<AbstractMesh*>& meshes,
                           std::vector<ParticleSystem*>& particleSystems,
                           std::vector<Skeleton*>& skeletons)>& onSuccess,
  const std::function<void()>& onError)
{
}

bool GLTFFileLoader::loadAsync(Scene* scene, const std::string& data,
                               const std::string& rootUrl,
                               const std::function<void()>& onsuccess,
                               const std::function<void()>& onerror)
{
}

void GLTFFileLoader::parseBuffers(const Json::object& parsedBuffers,
                                  IGLTFRuntime& gltfRuntime)
{
  for (auto& buf : parsedBuffers) {
    gltfRuntime.buffers[buf.first] = buf.second.get<Json::object>();
    ++gltfRuntime.buffersCount;
  }
}

void GLTFFileLoader::parseShaders(const Json::object& parsedShaders,
                                  IGLTFRuntime& gltfRuntime)
{
  for (auto& sha : parsedShaders) {
    gltfRuntime.buffers[sha.first] = sha.second.get<Json::object>();
    ++gltfRuntime.shaderscount;
  }
}

void GLTFFileLoader::parseObject(const Json::object& parsedObjects,
                                 const std::string& runtimeProperty,
                                 IGLTFRuntime& gltfRuntime)
{
}

void GLTFFileLoader::normalizeUVs(Float32Array& buffer) const
{
  if (buffer.empty()) {
    return;
  }

  for (size_t i = 0; i < buffer.size() / 2; ++i) {
    buffer[i * 2 + 1] = 1.f - buffer[i * 2 + 1];
  }
}

std::string
GLTFFileLoader::replaceInString(const std::string& str,
                                const std::string& searchValue,
                                const std::string& replaceValue) const
{
  std::string source = str;
  String::replaceInPlace(source, searchValue, replaceValue);
  return source;
}

std::string GLTFFileLoader::getAttribute(
  const IGLTFTechniqueParameter& attributeParameter) const
{
  if (attributeParameter.semantic == "NORMAL") {
    return "normal";
  }
  else if (attributeParameter.semantic == "POSITION") {
    return "position";
  }
  else if (attributeParameter.semantic == "JOINT") {
    return "matricesIndices";
  }
  else if (attributeParameter.semantic == "WEIGHT") {
    return "matricesWeights";
  }
  else if (attributeParameter.semantic == "COLOR") {
    return "color";
  }
  else if (attributeParameter.semantic.find("TEXCOORD_") != std::string::npos) {
    auto splitString = String::split(attributeParameter.semantic, '_');
    if (splitString.size() > 1) {
      int channel = std::stoi(splitString[1]);
      return "uv" + (channel == 0 ? "" : std::to_string(channel + 1));
    }
  }

  return "";
}

std::string GLTFFileLoader::getAnimationPath(const std::string& path) const
{
  auto it
    = std::find(glTFAnimationPaths.begin(), glTFAnimationPaths.end(), path);
  if (it != glTFAnimationPaths.end()) {
    size_t index = static_cast<size_t>(it - glTFAnimationPaths.begin());
    return babylonAnimationPaths[index];
  }

  return path;
}

void GLTFFileLoader::loadAnimations(const IGLTFRuntime& gltfRuntime) const
{
}

Matrix GLTFFileLoader::configureBoneTransformation(const IGLTFNode& node) const
{
  Matrix mat;
  if (!node.translation.empty() && !node.rotation.empty()
      && !node.scale.empty()) {
    auto scale    = Vector3::FromArray(node.scale);
    auto rotation = Quaternion::FromArray(node.rotation);
    auto position = Vector3::FromArray(node.translation);

    // Y is Up
    if (GLTFFileLoader::MakeYUP) {
      rotation = rotation.multiply(Quaternion(-0.707107f, 0.f, 0.f, 0.707107f));
    }

    mat = Matrix::Compose(scale, rotation, position);
  }
  else {
    mat = Matrix::FromArray(node.matrix);
  }

  return mat;
}

Bone* GLTFFileLoader::getParentBone(const IGLTFRuntime& gltfRuntime,
                                    const IGLTFSkins& skins,
                                    const std::string& jointName,
                                    Skeleton* newSkeleton) const
{
// Try to find
#if 0
  auto it = std::find_if(
    newSkeleton->bones.begin(), newSkeleton->bones.end(),
    [&jointName](const Bone* bone) { return bone->id == jointName; });
  if (it != newSkeleton->bones.end()) {
    return nullptr;
  }
#endif

  // Not found, search in gltf nodes
  for (auto& nde : gltfRuntime.nodes) {
    const IGLTFNode& node = nde.second;
    if (node.jointName.empty()) {
      continue;
    }

    for (auto& childId : node.children) {
      const IGLTFNode& child = gltfRuntime.nodes.at(childId);
      if (child.jointName.empty()) {
        continue;
      }

      if (child.jointName == jointName) {
        return nullptr;
      }
    }
  }

  return nullptr;
}

Bone* GLTFFileLoader::getNodeToRoot(const std::vector<INodeToRoot> nodesToRoot,
                                    const std::string& id) const
{
  for (auto& nodeToRoot : nodesToRoot) {
    for (auto& child : nodeToRoot.node.children) {
      if (child == id) {
        return nodeToRoot.bone;
      }
    }
  }
  return nullptr;
}

IJointNode GLTFFileLoader::getJointNode(const IGLTFRuntime& gltfRuntime,
                                        const std::string& jointName) const
{
  if (std_util::contains(gltfRuntime.nodes, jointName)) {
    return IJointNode(gltfRuntime.nodes.at(jointName), jointName);
  }

  for (auto& nde : gltfRuntime.nodes) {
    const IGLTFNode& node = nde.second;
    if (node.jointName == jointName) {
      return IJointNode(node, nde.first);
    }
  }

  return IJointNode();
}

bool GLTFFileLoader::nodeIsInJoints(const IGLTFSkins& skins,
                                    const std::string& id) const
{
  return (std::find(skins.jointNames.begin(), skins.jointNames.end(), id)
          != skins.jointNames.end());
}

void GLTFFileLoader::getNodesToRoot(const IGLTFRuntime& gltfRuntime,
                                    Skeleton* newSkeleton,
                                    const IGLTFSkins& skins,
                                    std::vector<INodeToRoot>& nodesToRoot) const
{
  // Creates nodes for root
  for (auto& nde : gltfRuntime.nodes) {
    const std::string& id = nde.first;
    const IGLTFNode& node = nde.second;

    if (node.jointName.empty() || nodeIsInJoints(skins, node.jointName)) {
      continue;
    }

    // Create node to root bone
    // Matrix mat = configureBoneTransformation(node);
    Bone* bone = nullptr;
    bone->id   = id;
    nodesToRoot.emplace_back(INodeToRoot(bone, node, id));
  }

  // Parenting
  for (auto& nodeToRoot : nodesToRoot) {
    for (auto& child : nodeToRoot.node.children) {
      for (auto& _nodeToRoot : nodesToRoot) {
        if (_nodeToRoot.id == child) {
          _nodeToRoot.bone->setParent(nodeToRoot.bone);
          nodeToRoot.bone->children.emplace_back(_nodeToRoot.bone);
          break;
        }
      }
    }
  }
}

void GLTFFileLoader::printMat(const Float32Array& m) const
{
  std::ostringstream oss;
  oss << m[0] << "\t" << m[1] << "\t" << m[2] << "\t" << m[3] << "\n";
  oss << m[4] << "\t" << m[5] << "\t" << m[6] << "\t" << m[7] << "\n";
  oss << m[8] << "\t" << m[9] << "\t" << m[10] << "\t" << m[11] << "\n";
  oss << m[12] << "\t" << m[13] << "\t" << m[14] << "\t" << m[15] << "\n";
  BABYLON_LOG_INFO("GLTFFileLoader", oss.str());
}

Skeleton* GLTFFileLoader::importSkeleton(const IGLTFRuntime& gltfRuntime,
                                         const IGLTFSkins& skins, Mesh* mesh,
                                         Skeleton* newSkeleton) const
{
  return nullptr;
}

Mesh* GLTFFileLoader::importMesh(const IGLTFRuntime& gltfRuntime,
                                 const IGLTFNode& node,
                                 std::vector<std::string> meshes,
                                 const std::string& id, Mesh* newMesh) const
{
  return nullptr;
}

void GLTFFileLoader::configureNode(Mesh* newNode, const Vector3& position,
                                   const Quaternion& rotation,
                                   const Vector3& scaling) const
{
  newNode->setPosition(position);
  newNode->setRotationQuaternion(rotation);
  newNode->setScaling(scaling);
}

void GLTFFileLoader::configureNodeFromMatrix(Mesh* newNode,
                                             const IGLTFNode& node,
                                             Node* /*parent*/) const
{
  if (!node.matrix.empty()) {
    Vector3 position(0.f, 0.f, 0.f);
    Quaternion rotation;
    Vector3 scaling(0.f, 0.f, 0.f);
    auto mat = Matrix::FromArray(node.matrix);
    mat.decompose(scaling, rotation, position);

    configureNode(newNode, position, rotation, scaling);
    newNode->computeWorldMatrix(true);
  }
  else {
    configureNode(newNode, Vector3::FromArray(node.translation),
                  Quaternion::FromArray(node.rotation),
                  Vector3::FromArray(node.scale));
  }
}

Node* GLTFFileLoader::importNode(const IGLTFRuntime& gltfRuntime,
                                 const IGLTFNode& node,
                                 const std::string& id) const
{
}

void GLTFFileLoader::traverseNodes(const IGLTFRuntime& gltfRuntime,
                                   const std::string& id, Node* parent,
                                   bool meshIncluded) const
{
}

void GLTFFileLoader::postLoad(const IGLTFRuntime& gltfRuntime) const
{
}

void GLTFFileLoader::onBindShaderMaterial(Mesh* mesh,
                                          const IGLTFRuntime& gltfRuntime,
                                          const std::string& unTreatedUniforms,
                                          ShaderMaterial* shaderMaterial,
                                          const IGLTFTechnique& technique,
                                          const IGLTFMaterial& material) const
{
}

void GLTFFileLoader::prepareShaderMaterialUniforms(
  const IGLTFRuntime& gltfRuntime, ShaderMaterial* shaderMaterial,
  const IGLTFTechnique& technique, const IGLTFMaterial& material,
  const std::string& unTreatedUniforms) const
{
}

void GLTFFileLoader::onShaderCompileError(const IGLTFProgram& program,
                                          ShaderMaterial* shaderMaterial) const
{
}

void GLTFFileLoader::onShaderCompileSuccess(
  const IGLTFRuntime& gltfRuntime, ShaderMaterial* shaderMaterial,
  const IGLTFTechnique& technique, const IGLTFMaterial& material,
  const std::string& unTreatedUniforms) const
{
}

std::string
GLTFFileLoader::parseShaderUniforms(const Tokenizer& tokenizer,
                                    const IGLTFTechnique& technique,
                                    const std::string& unTreatedUniforms) const
{
  return "";
}

void GLTFFileLoader::importMaterials(const IGLTFRuntime& gltfRuntime)
{
}

void GLTFFileLoader::_loadShadersAsync(const IGLTFRuntime& gltfRuntime,
                                       const std::function<void()>& onload)
{
}

void GLTFFileLoader::_loadBuffersAsync(const IGLTFRuntime& gltfRuntime,
                                       const std::function<void()>& onload)
{
}

void GLTFFileLoader::_createNodes(const IGLTFRuntime& gltfRuntime) const
{
}

} // end of namespace BABYLON
