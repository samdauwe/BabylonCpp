#include <babylon/loading/glTF/2.0/gltf_loader_interfaces.h>

#include <babylon/core/json_util.h>

namespace BABYLON {
namespace GLTF2 {

IAccessor IAccessor::Parse(const json& parsedAccessor)
{
  IAccessor accessor;

  // Buffer view
  if (json_util::has_valid_key_value(parsedAccessor, "bufferView")) {
    accessor.bufferView
      = json_util::get_number<size_t>(parsedAccessor, "bufferView");
  }

  // Byte offset
  if (json_util::has_valid_key_value(parsedAccessor, "byteOffset")) {
    accessor.byteOffset
      = json_util::get_number<size_t>(parsedAccessor, "byteOffset");
  }

  // Component type
  accessor.componentType = IGLTF2::EnumUtils::NumberToAccessorComponentType(
    json_util::get_number<size_t>(parsedAccessor, "componentType"));

  // Normalized
  if (json_util::has_valid_key_value(parsedAccessor, "normalized")) {
    accessor.normalized = json_util::get_bool(parsedAccessor, "normalized");
  }

  // Count
  accessor.count = json_util::get_number<size_t>(parsedAccessor, "count");

  // Type
  accessor.type = IGLTF2::EnumUtils::StringToAccessorType(
    json_util::get_string(parsedAccessor, "type"));

  // Max
  accessor.max = json_util::get_array<int32_t>(parsedAccessor, "max");

  // Min
  accessor.min = json_util::get_array<int32_t>(parsedAccessor, "min");

  // Sparse
  if (json_util::has_valid_key_value(parsedAccessor, "sparse")) {
    accessor.sparse = IGLTF2::IAccessorSparse::Parse(parsedAccessor["sparse"]);
  }

  return accessor;
}

IGLTF2::IAccessorSparsePtr
IGLTF2::IAccessorSparse::Parse(const json& parsedAccessorSparse)
{
  auto accessorSparse = std::make_shared<IAccessorSparse>();

  // Count
  accessorSparse->count
    = json_util::get_number<size_t>(parsedAccessorSparse, "count");

  // Indices
  if (json_util::has_valid_key_value(parsedAccessorSparse, "indices")) {
    accessorSparse->indices
      = IAccessorSparseIndices::Parse(parsedAccessorSparse["indices"]);
  }

  // Values
  if (json_util::has_valid_key_value(parsedAccessorSparse, "values")) {
    accessorSparse->values
      = IAccessorSparseValues::Parse(parsedAccessorSparse["values"]);
  }

  return accessorSparse;
}

IGLTF2::IAccessorSparseIndices
IGLTF2::IAccessorSparseIndices::Parse(const json& parsedAccessorSparseIndices)
{
  IAccessorSparseIndices accessorSparseIndices;

  // Buffer view
  accessorSparseIndices.bufferView
    = json_util::get_number<size_t>(parsedAccessorSparseIndices, "bufferView");

  // Byte offset
  if (json_util::has_valid_key_value(parsedAccessorSparseIndices,
                                     "byteOffset")) {
    accessorSparseIndices.byteOffset = json_util::get_number<size_t>(
      parsedAccessorSparseIndices, "byteOffset");
  }

  // Component type
  accessorSparseIndices.componentType
    = IGLTF2::EnumUtils::NumberToAccessorComponentType(
      json_util::get_number<size_t>(parsedAccessorSparseIndices,
                                    "componentType"));

  return accessorSparseIndices;
}

IGLTF2::IAccessorSparseValues
IGLTF2::IAccessorSparseValues::Parse(const json& parsedAccessorSparseValues)
{
  IAccessorSparseValues accessorSparseValues;

  // Buffer view
  accessorSparseValues.bufferView
    = json_util::get_number<size_t>(parsedAccessorSparseValues, "bufferView");

  // Byte offset
  if (json_util::has_valid_key_value(parsedAccessorSparseValues,
                                     "byteOffset")) {
    accessorSparseValues.byteOffset
      = json_util::get_number<size_t>(parsedAccessorSparseValues, "byteOffset");
  }

  return accessorSparseValues;
}

IGLTF2::IAsset IGLTF2::IAsset::Parse(const json& parsedAsset)
{
  IGLTF2::IAsset asset;

  // Copyright
  asset.copyright = json_util::get_string(parsedAsset, "copyright");

  // Generator
  asset.generator = json_util::get_string(parsedAsset, "generator");

  // Version
  asset.version = json_util::get_string(parsedAsset, "version");

  // Minimum version
  asset.minVersion = json_util::get_string(parsedAsset, "minVersion");

  return asset;
}

IBuffer IBuffer::Parse(const json& parsedBuffer)
{
  IBuffer buffer;

  // Uri
  buffer.uri = json_util::get_string(parsedBuffer, "uri");

  // Byte length
  buffer.byteLength = json_util::get_number<size_t>(parsedBuffer, "byteLength");

  return buffer;
}

IBufferView IBufferView::Parse(const json& parsedBufferView)
{
  IBufferView bufferView;

  // Buffer
  bufferView.buffer = json_util::get_number<size_t>(parsedBufferView, "buffer");

  // Byte offset
  if (json_util::has_valid_key_value(parsedBufferView, "byteOffset")) {
    bufferView.byteOffset
      = json_util::get_number<size_t>(parsedBufferView, "byteOffset");
  }

  // Byte length
  bufferView.byteLength
    = json_util::get_number<size_t>(parsedBufferView, "byteLength");

  // Byte stride
  if (json_util::has_valid_key_value(parsedBufferView, "byteStride")) {
    bufferView.byteStride
      = json_util::get_number<size_t>(parsedBufferView, "byteStride");
  }

  return bufferView;
}

IGLTF2::ICameraOrthographicPtr
IGLTF2::ICameraOrthographic::Parse(const json& parsedCamera)
{
  auto camera = std::make_shared<ICameraOrthographic>();

  // xmag
  camera->xmag = json_util::get_number<float>(parsedCamera, "xmag");

  // ymag
  camera->ymag = json_util::get_number<float>(parsedCamera, "ymag");

  // zfar
  camera->zfar = json_util::get_number<float>(parsedCamera, "zfar");

  // znear
  camera->znear = json_util::get_number<float>(parsedCamera, "znear");

  return camera;
}

IGLTF2::ICameraPerspectivePtr
IGLTF2::ICameraPerspective::Parse(const json& parsedCamera)
{
  auto camera = std::make_shared<ICameraPerspective>();

  // Aspect ratio
  if (json_util::has_valid_key_value(parsedCamera, "aspectRatio")) {
    camera->aspectRatio
      = json_util::get_number<float>(parsedCamera, "aspectRatio");
  }

  // yfov
  camera->yfov = json_util::get_number<float>(parsedCamera, "yfov");

  // zfar
  if (json_util::has_valid_key_value(parsedCamera, "zfar")) {
    camera->zfar = json_util::get_number<float>(parsedCamera, "zfar");
  }

  // znear
  camera->znear = json_util::get_number<float>(parsedCamera, "znear");

  return camera;
}

ICamera ICamera::Parse(const json& parsedCamera)
{
  ICamera camera;

  // Orthographic
  if (json_util::has_valid_key_value(parsedCamera, "orthographic")) {
    camera.orthographic
      = IGLTF2::ICameraOrthographic::Parse(parsedCamera["orthographic"]);
  }

  // Perspective
  if (json_util::has_valid_key_value(parsedCamera, "perspective")) {
    camera.perspective
      = IGLTF2::ICameraPerspective::Parse(parsedCamera["perspective"]);
  }

  // Camera type
  camera.type = IGLTF2::EnumUtils::StringToCameraType(
    json_util::get_string(parsedCamera, "type"));

  return camera;
}

IImage IImage::Parse(const json& parsedImage)
{
  IImage image;

  // Uri
  image.uri = json_util::get_string(parsedImage, "uri");

  // Mime type
  if (json_util::has_valid_key_value(parsedImage, "mimeType")) {
    image.mimeType
      = IGLTF2::EnumUtils::StringToImageMimeType(parsedImage["mimeType"]);
  }

  // Buffer view
  if (json_util::has_valid_key_value(parsedImage, "bufferView")) {
    image.bufferView = json_util::get_number<size_t>(parsedImage, "bufferView");
  }

  return image;
}

IMaterial IMaterial::Parse(const json& parsedMaterial)
{
  IMaterial material;

  // PBR metallic roughness
  if (json_util::has_valid_key_value(parsedMaterial, "pbrMetallicRoughness")) {
    material.pbrMetallicRoughness = IMaterialPbrMetallicRoughness::Parse(
      parsedMaterial["pbrMetallicRoughness"]);
  }

  // Normal texture
  if (json_util::has_valid_key_value(parsedMaterial, "normalTexture")) {
    material.normalTexture
      = IMaterialNormalTextureInfo::Parse(parsedMaterial["normalTexture"]);
  }

  // Occlusion texture
  if (json_util::has_valid_key_value(parsedMaterial, "occlusionTexture")) {
    material.occlusionTexture = IMaterialOcclusionTextureInfo::Parse(
      parsedMaterial["occlusionTexture"]);
  }

  // Emissive texture
  if (json_util::has_valid_key_value(parsedMaterial, "emissiveTexture")) {
    material.emissiveTexture
      = ITextureInfo::Parse(parsedMaterial["emissiveTexture"]);
  }

  // Emissive factor
  material.emissiveFactor
    = json_util::get_array<float>(parsedMaterial, "emissiveFactor");

  // Alpha mode
  if (json_util::has_valid_key_value(parsedMaterial, "alphaMode")) {
    material.alphaMode = IGLTF2::EnumUtils::StringToMaterialAlphaMode(
      json_util::get_string(parsedMaterial, "alphaMode"));
  }

  // Alpha Cutoff
  if (json_util::has_valid_key_value(parsedMaterial, "alphaCutoff")) {
    material.alphaCutoff
      = json_util::get_number<float>(parsedMaterial, "alphaCutoff");
  }

  // Double sided
  if (json_util::has_valid_key_value(parsedMaterial, "doubleSided")) {
    material.doubleSided = json_util::get_bool(parsedMaterial, "doubleSided");
  }

  // Name
  if (json_util::has_valid_key_value(parsedMaterial, "name")) {
    material.name = json_util::get_string(parsedMaterial, "name");
  }

  return material;
}

IMaterialNormalTextureInfoPtr
IMaterialNormalTextureInfo::Parse(const json& parsedMaterialNormalTextureInfo)
{
  auto materialNormalTextureInfo
    = std::make_shared<IMaterialNormalTextureInfo>();

  // Scale
  if (json_util::has_valid_key_value(parsedMaterialNormalTextureInfo,
                                     "scale")) {
    materialNormalTextureInfo->scale
      = json_util::get_number<float>(parsedMaterialNormalTextureInfo, "scale");
  }

  return materialNormalTextureInfo;
}

IMaterialOcclusionTextureInfoPtr IMaterialOcclusionTextureInfo::Parse(
  const json& parsedMaterialOcclusionTextureInfo)
{
  auto materialOcclusionTextureInfo
    = std::make_shared<IMaterialOcclusionTextureInfo>();

  // Scale
  if (json_util::has_valid_key_value(parsedMaterialOcclusionTextureInfo,
                                     "strength")) {
    materialOcclusionTextureInfo->strength = json_util::get_number<float>(
      parsedMaterialOcclusionTextureInfo, "strength");
  }

  return materialOcclusionTextureInfo;
}

IMaterialPbrMetallicRoughnessPtr IMaterialPbrMetallicRoughness::Parse(
  const json& parsedMaterialPbrMetallicRoughness)
{
  auto materialPbrMetallicRoughness
    = std::make_shared<IMaterialPbrMetallicRoughness>();

  // Base color factor
  materialPbrMetallicRoughness->baseColorFactor = json_util::get_array<float>(
    parsedMaterialPbrMetallicRoughness, "baseColorFactor");

  // Base color texture
  if (json_util::has_valid_key_value(parsedMaterialPbrMetallicRoughness,
                                     "baseColorTexture")) {
    materialPbrMetallicRoughness->baseColorTexture = ITextureInfo::Parse(
      parsedMaterialPbrMetallicRoughness["baseColorTexture"]);
  }

  // Metallic factor
  if (json_util::has_valid_key_value(parsedMaterialPbrMetallicRoughness,
                                     "metallicFactor")) {
    materialPbrMetallicRoughness->metallicFactor = json_util::get_number<float>(
      parsedMaterialPbrMetallicRoughness, "metallicFactor");
  }

  // Roughness factor
  if (json_util::has_valid_key_value(parsedMaterialPbrMetallicRoughness,
                                     "roughnessFactor")) {
    materialPbrMetallicRoughness->roughnessFactor
      = json_util::get_number<float>(parsedMaterialPbrMetallicRoughness,
                                     "roughnessFactor");
  }

  // Metallic roughness texture
  if (json_util::has_valid_key_value(parsedMaterialPbrMetallicRoughness,
                                     "metallicRoughnessTexture")) {
    materialPbrMetallicRoughness->metallicRoughnessTexture
      = ITextureInfo::Parse(
        parsedMaterialPbrMetallicRoughness["metallicRoughnessTexture"]);
  }

  return materialPbrMetallicRoughness;
}

IMesh IMesh::Parse(const json& parsedMesh)
{
  IMesh mesh;

  // Primitives
  for (const auto& meshPrimitive :
       json_util::get_array<json>(parsedMesh, "primitives")) {
    mesh.primitives.emplace_back(IMeshPrimitive::Parse(meshPrimitive));
  }

  // Weights
  mesh.weights = json_util::get_array<float>(parsedMesh, "weights");

  // Name
  if (json_util::has_valid_key_value(parsedMesh, "name")) {
    mesh.name = json_util::get_string(parsedMesh, "name");
  }

  return mesh;
}

IMeshPrimitive IMeshPrimitive::Parse(const json& parsedMeshPrimitive)
{
  IMeshPrimitive meshPrimitive;

  // Attributes
  if (json_util::has_valid_key_value(parsedMeshPrimitive, "attributes")) {
    for (const auto& attributeElement :
         parsedMeshPrimitive["attributes"].items()) {
      auto key   = attributeElement.key();
      auto value = attributeElement.value();
      if (value.is_number_unsigned()) {
        meshPrimitive.attributes[key] = value.get<size_t>();
      }
    }
  }

  // Indices
  if (json_util::has_valid_key_value(parsedMeshPrimitive, "indices")) {
    meshPrimitive.indices
      = json_util::get_number<size_t>(parsedMeshPrimitive, "indices");
  }

  // Material
  if (json_util::has_valid_key_value(parsedMeshPrimitive, "material")) {
    meshPrimitive.material
      = json_util::get_number<size_t>(parsedMeshPrimitive, "material");
  }

  // Mode
  if (json_util::has_valid_key_value(parsedMeshPrimitive, "mode")) {
    meshPrimitive.mode = IGLTF2::EnumUtils::NumberToMeshPrimitiveMode(
      json_util::get_number<size_t>(parsedMeshPrimitive, "mode"));
  }

  return meshPrimitive;
}

INodePtr INode::Parse(const json& parsedNode)
{
  auto node = std::make_shared<INode>();

  // Camera
  if (json_util::has_valid_key_value(parsedNode, "camera")) {
    node->camera = json_util::get_number<size_t>(parsedNode, "camera");
  }

  // Children
  node->children = json_util::get_array<uint32_t>(parsedNode, "children");

  // Skin
  if (json_util::has_valid_key_value(parsedNode, "skin")) {
    node->skin = json_util::get_number<size_t>(parsedNode, "skin");
  }

  // Matrix
  node->matrix = json_util::get_array<float>(parsedNode, "matrix");

  // Mesh
  if (json_util::has_valid_key_value(parsedNode, "mesh")) {
    node->mesh = json_util::get_number<size_t>(parsedNode, "mesh");
  }

  // Rotation
  node->rotation = json_util::get_array<float>(parsedNode, "rotation");

  // Scale
  node->scale = json_util::get_array<float>(parsedNode, "scale");

  // Translation
  node->translation = json_util::get_array<float>(parsedNode, "translation");

  // Weights
  node->weights = json_util::get_array<float>(parsedNode, "weights");

  return node;
}

ISampler ISampler::Parse(const json& parsedSampler)
{
  ISampler sampler;

  // Mag filter
  if (json_util::has_valid_key_value(parsedSampler, "magFilter")) {
    sampler.magFilter = IGLTF2::EnumUtils::NumberToTextureMagFilter(
      json_util::get_number<size_t>(parsedSampler, "magFilter"));
  }

  // Min filter
  if (json_util::has_valid_key_value(parsedSampler, "minFilter")) {
    sampler.minFilter = IGLTF2::EnumUtils::NumberToTextureMinFilter(
      json_util::get_number<size_t>(parsedSampler, "minFilter"));
  }

  // Wrap S
  if (json_util::has_valid_key_value(parsedSampler, "wrapS")) {
    sampler.wrapS = IGLTF2::EnumUtils::NumberToTextureWrapMode(
      json_util::get_number<size_t>(parsedSampler, "wrapS"));
  }

  // Wrap T
  if (json_util::has_valid_key_value(parsedSampler, "wrapT")) {
    sampler.wrapT = IGLTF2::EnumUtils::NumberToTextureWrapMode(
      json_util::get_number<size_t>(parsedSampler, "wrapT"));
  }

  return sampler;
}

IScene IScene::Parse(const json& parsedScene)
{
  IScene scene;

  // Nodes
  scene.nodes = json_util::get_array<size_t>(parsedScene, "nodes");

  return scene;
}

ITexture ITexture::Parse(const json& parsedTexture)
{
  ITexture texture;

  // Sampler
  if (json_util::has_valid_key_value(parsedTexture, "sampler")) {
    texture.sampler = json_util::get_number<size_t>(parsedTexture, "sampler");
  }

  // Source
  texture.source = json_util::get_number<size_t>(parsedTexture, "source");

  return texture;
}

ITextureInfoPtr ITextureInfo::Parse(const json& parsedTextureInfo)
{
  auto textureInfo = std::make_shared<ITextureInfo>();

  // Index
  textureInfo->index
    = json_util::get_number<size_t>(parsedTextureInfo, "index");

  // Tex coord
  if (json_util::has_valid_key_value(parsedTextureInfo, "texCoord")) {
    textureInfo->texCoord
      = json_util::get_number<unsigned int>(parsedTextureInfo, "texCoord");
  }

  return textureInfo;
}

std::unique_ptr<IGLTF> IGLTF::Parse(const json& parsedGLTFObject)
{
  auto glTFObjectPtr = std::make_unique<IGLTF>();
  auto& glTFObject   = *glTFObjectPtr;

  // Accessors
  for (const auto& accessor :
       json_util::get_array<json>(parsedGLTFObject, "accessors")) {
    glTFObject.accessors.emplace_back(IAccessor::Parse(accessor));
  }

  // Asset
  if (json_util::has_valid_key_value(parsedGLTFObject, "asset")) {
    glTFObject.asset = IGLTF2::IAsset::Parse(parsedGLTFObject["asset"]);
  }

  // Buffers
  for (const auto& buffer :
       json_util::get_array<json>(parsedGLTFObject, "buffers")) {
    glTFObject.buffers.emplace_back(IBuffer::Parse(buffer));
  }

  // Buffer views
  for (const auto& bufferView :
       json_util::get_array<json>(parsedGLTFObject, "bufferViews")) {
    glTFObject.bufferViews.emplace_back(IBufferView::Parse(bufferView));
  }

  // Cameras
  for (const auto& camera :
       json_util::get_array<json>(parsedGLTFObject, "cameras")) {
    glTFObject.cameras.emplace_back(ICamera::Parse(camera));
  }

  // Images
  for (const auto& image :
       json_util::get_array<json>(parsedGLTFObject, "images")) {
    glTFObject.images.emplace_back(IImage::Parse(image));
  }

  // Materials
  for (const auto& material :
       json_util::get_array<json>(parsedGLTFObject, "materials")) {
    glTFObject.materials.emplace_back(IMaterial::Parse(material));
  }

  // Meshes
  for (const auto& mesh :
       json_util::get_array<json>(parsedGLTFObject, "meshes")) {
    glTFObject.meshes.emplace_back(IMesh::Parse(mesh));
  }

  // Nodes
  for (const auto& node :
       json_util::get_array<json>(parsedGLTFObject, "nodes")) {
    glTFObject.nodes.emplace_back(INode::Parse(node));
  }

  // Samplers
  for (const auto& sampler :
       json_util::get_array<json>(parsedGLTFObject, "samplers")) {
    glTFObject.samplers.emplace_back(ISampler::Parse(sampler));
  }

  // Scene
  if (json_util::has_valid_key_value(parsedGLTFObject, "scene")) {
    glTFObject.scene = json_util::get_number<size_t>(parsedGLTFObject, "scene");
  }

  // Scenes
  for (const auto& scene :
       json_util::get_array<json>(parsedGLTFObject, "scenes")) {
    glTFObject.scenes.emplace_back(IScene::Parse(scene));
  }

  // Textures
  for (const auto& texture :
       json_util::get_array<json>(parsedGLTFObject, "textures")) {
    glTFObject.textures.emplace_back(ITexture::Parse(texture));
  }

  return glTFObjectPtr;
}

} // end of namespace GLTF2
} // end of namespace BABYLON
