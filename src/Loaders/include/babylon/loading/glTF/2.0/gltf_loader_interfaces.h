#ifndef BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_INTERFACES_H
#define BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_INTERFACES_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/tools/observable.h>

namespace BABYLON {
namespace GLTF2 {

/**
 * Enums
 */
enum class EComponentType {
  BYTE           = 5120,
  UNSIGNED_BYTE  = 5121,
  SHORT          = 5122,
  UNSIGNED_SHORT = 5123,
  UNSIGNED_INT   = 5125,
  FLOAT          = 5126
}; // end of class EComponentType

enum class EMeshPrimitiveMode {
  POINTS         = 0,
  LINES          = 1,
  LINE_LOOP      = 2,
  LINE_STRIP     = 3,
  TRIANGLES      = 4,
  TRIANGLE_STRIP = 5,
  TRIANGLE_FAN   = 6
}; // end of enum class EMeshPrimitiveMode

enum class ETextureMagFilter {
  NEAREST = 9728,
  LINEAR  = 9729,
}; // end of enum class ETextureMagFilter

enum class ETextureMinFilter {
  NEAREST                = 9728,
  LINEAR                 = 9729,
  NEAREST_MIPMAP_NEAREST = 9984,
  LINEAR_MIPMAP_NEAREST  = 9985,
  NEAREST_MIPMAP_LINEAR  = 9986,
  LINEAR_MIPMAP_LINEAR   = 9987
}; // end of enum class ETextureMinFilter

enum class ETextureWrapMode {
  CLAMP_TO_EDGE   = 33071,
  MIRRORED_REPEAT = 33648,
  REPEAT          = 10497
}; // end of enum class ETextureWrapMode

/**
 * Interfaces
 */
struct IGLTFProperty {
  unordered_map_t<string_t, string_t> extensions;
  string_t extras;
}; // end of struct IGLTFProperty

struct IGLTFChildRootProperty : public IGLTFProperty {
  string_t name;
}; // end of struct IGLTFChildRootProperty

struct IGLTFAccessorSparseIndices : public IGLTFProperty {
  int bufferView;
  int byteOffset;
  EComponentType componentType;
}; // end of struct IGLTFAccessorSparseIndices

struct IGLTFAccessorSparseValues : public IGLTFProperty {
  int bufferView;
  int byteOffset;
}; // end of struct IGLTFAccessorSparseValues

struct IGLTFAccessorSparse : public IGLTFProperty {
  int count;
  IGLTFAccessorSparseIndices indices;
  IGLTFAccessorSparseValues values;
}; // end of struct IGLTFAccessorSparse

struct IGLTFAccessor : public IGLTFChildRootProperty {
  unsigned int bufferView;
  Nullable<unsigned int> byteOffset;
  EComponentType componentType;
  bool normalized;
  unsigned int count;
  string_t type;
  Float32Array max;
  Float32Array min;
  Nullable<IGLTFAccessorSparse> sparse;
  // Runtime values
  int index;
}; // end of struct IGLTFAccessor

struct IGLTFAnimationChannelTarget : public IGLTFProperty {
  unsigned int node;
  string_t path;
}; // end of struct IGLTFAnimationChannelTarget

struct IGLTFAnimationChannel : public IGLTFProperty {
  unsigned int sampler;
  IGLTFAnimationChannelTarget target;
}; // end of struct IGLTFAnimationChannel

struct IGLTFAnimationSampler : public IGLTFProperty {
  unsigned int input;
  string_t interpolation;
  unsigned int output;
}; // end of struct IGLTFAnimationSampler

struct IGLTFAnimation : public IGLTFChildRootProperty {
  vector_t<IGLTFAnimationChannel> channels;
  vector_t<IGLTFAnimationSampler> samplers;

  // Runtime values
  int index;
  vector_t<string_t> targets;
}; // end of struct IGLTFAnimation

struct IGLTFAsset : public IGLTFChildRootProperty {
  string_t copyright;
  string_t generator;
  string_t version;
  string_t minVersion;
}; // end of struct IGLTFAsset

struct IGLTFBuffer : public IGLTFChildRootProperty {
  string_t uri;
  int byteLength;
  // Runtime values
  int index;
  Nullable<ArrayBufferView> loadedData;
  Observable<IGLTFBuffer>::SPtr loadedObservable;
}; // end of struct IGLTFBuffer

struct IGLTFBufferView : public IGLTFChildRootProperty {
  unsigned int buffer;
  int byteOffset;
  int byteLength;
  unsigned int byteStride;
  // Runtime values
  int index;
}; // end of struct IGLTFBufferView

struct IGLTFCameraOrthographic : public IGLTFProperty {
  float xmag;
  float ymag;
  float zfar;
  float znear;
}; // end of struct IGLTFCameraOrthographic

struct IGLTFCameraPerspective : public IGLTFProperty {
  float aspectRatio;
  float yfov;
  float zfar;
  float znear;
}; // end of struct IGLTFCameraPerspective

struct IGLTFCamera : public IGLTFChildRootProperty {
  IGLTFCameraOrthographic orthographic;
  IGLTFCameraPerspective perspective;
  string_t type;
}; // end of struct IGLTFCamera

struct IGLTFImage : public IGLTFChildRootProperty {
  string_t uri;
  string_t mimeType;
  int bufferView;
  // Runtime values
  int index;
}; // end of struct IGLTFImage

struct IGLTFTextureInfo {
  unsigned int index;
  unsigned int texCoord;
}; // end of struct IGLTFTextureInfo

struct IGLTFMaterialNormalTextureInfo : public IGLTFTextureInfo {
  Nullable<float> scale;
}; // end of struct IGLTFMaterialNormalTextureInfo

struct IGLTFMaterialOcclusionTextureInfo : public IGLTFTextureInfo {
  Nullable<float> strength;
}; // end of struct IGLTFMaterialOcclusionTextureInfo

struct IGLTFMaterialPbrMetallicRoughness {
  Float32Array baseColorFactor;
  Nullable<IGLTFTextureInfo> baseColorTexture;
  Nullable<float> metallicFactor;
  Nullable<float> roughnessFactor;
  Nullable<IGLTFTextureInfo> metallicRoughnessTexture;
}; // end of struct IGLTFMaterialPbrMetallicRoughness

struct IGLTFMaterial : public IGLTFChildRootProperty {
  Nullable<IGLTFMaterialPbrMetallicRoughness> pbrMetallicRoughness;
  Nullable<IGLTFMaterialNormalTextureInfo> normalTexture;
  Nullable<IGLTFMaterialOcclusionTextureInfo> occlusionTexture;
  Nullable<IGLTFTextureInfo> emissiveTexture;
  Float32Array emissiveFactor;
  string_t alphaMode;
  Nullable<float> alphaCutoff;
  bool doubleSided;
  // Runtime values
  int index;
  Material* babylonMaterial;
}; // end of struct IGLTFMaterial

struct IGLTFMeshPrimitive : public IGLTFProperty {
  unordered_map_t<string_t, int> attributes;
  int indices;
  int material;
  EMeshPrimitiveMode mode;
  vector_t<unordered_map_t<string_t, int>> targets;
  // Runtime values
  VertexData* vertexData;
  vector_t<VertexData> targetsVertexData;
}; // end of struct IGLTFMeshPrimitive

struct IGLTFMesh : public IGLTFChildRootProperty {
  vector_t<IGLTFMeshPrimitive> primitives;
  Float32Array weights;
  // Runtime values
  int index;
}; // end of struct IGLTFMesh

struct IGLTFNode : public IGLTFChildRootProperty {
  int camera;
  Float32Array children;
  int skin;
  Float32Array matrix;
  int mesh;
  Float32Array rotation;
  Float32Array scale;
  Float32Array translation;
  Float32Array weights;
  // Runtime values
  int index;
  IGLTFNode* parent;
  Mesh* babylonMesh;
  unordered_map_t<int, Bone*> babylonBones;
  vector_t<Node*> babylonAnimationTargets;
}; // end of struct IGLTFNode

struct IGLTFSampler : public IGLTFChildRootProperty {
  ETextureMagFilter magFilter;
  ETextureMagFilter minFilter;
  ETextureWrapMode wrapS;
  ETextureWrapMode wrapT;
}; // end of struct IGLTFSampler

struct IGLTFScene : public IGLTFChildRootProperty {
  Float32Array nodes;
  // Runtime values
  int index;
}; // end of struct IGLTFScene

struct IGLTFSkin : public IGLTFChildRootProperty {
  Nullable<unsigned int> inverseBindMatrices;
  int skeleton;
  Uint32Array joints;
  // Runtime values
  int index;
  Skeleton* babylonSkeleton;
}; // end of struct IGLTFSkin

struct IGLTFTexture : public IGLTFChildRootProperty {
  int sampler;
  int source;
  // Runtime values
  int index;
  string_t url;
  Observable<IGLTFTexture> dataReadyObservable;
}; // end of struct IGLTFTexture

struct IGLTF : public IGLTFProperty {
  vector_t<IGLTFAccessor> accessors;
  vector_t<IGLTFAnimation> animations;
  vector_t<IGLTFAsset> asset;
  vector_t<IGLTFBuffer> buffers;
  vector_t<IGLTFBufferView> bufferViews;
  vector_t<IGLTFCamera> cameras;
  vector_t<string_t> extensionsUsed;
  vector_t<string_t> extensionsRequired;
  vector_t<IGLTFImage> images;
  vector_t<IGLTFMaterial> materials;
  vector_t<IGLTFMesh> meshes;
  vector_t<IGLTFNode> nodes;
  vector_t<IGLTFSampler> sampler;
  int scene;
  vector_t<IGLTFScene> scenes;
  vector_t<IGLTFSkin> skins;
  vector_t<IGLTFTexture> textures;
}; // end of struct IGLTF

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_INTERFACES_H
