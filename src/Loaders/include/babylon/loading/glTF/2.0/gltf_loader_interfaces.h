#ifndef BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_INTERFACES_H
#define BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_INTERFACES_H

#include <optional>

#include <babylon/loading/glTF/2.0/gltf2_interface.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

class AbstractMesh;
class AnimationGroup;
struct ArrayBufferView;
class Bone;
class Material;
class Mesh;
class Skeleton;
class TransformNode;
using AbstractMeshPtr   = std::shared_ptr<AbstractMesh>;
using AnimationGroupPtr = std::shared_ptr<AnimationGroup>;
using BonePtr           = std::shared_ptr<Bone>;
using MaterialPtr       = std::shared_ptr<Material>;
using MeshPtr           = std::shared_ptr<Mesh>;
using SkeletonPtr       = std::shared_ptr<Skeleton>;
using TransformNodePtr  = std::shared_ptr<TransformNode>;

namespace GLTF2 {

/**
 * @brief Loader interface with an index field.
 */
struct IArrayItem {
  /**
   * The index of this item in the array.
   */
  size_t index;
}; // end of struct IArrayItem

/**
 * @brief Loader interface with additional members.
 */
struct IAccessor : public IGLTF2::IAccessor, IArrayItem {
  /** @hidden */
  std::optional<ArrayBufferView> _data;

  /** @hidden */
  std::optional<VertexBuffer> _babylonVertexBuffer;
}; // end of struct IAccessor

/**
 * @brief Loader interface with additional members.
 */
struct IAnimationChannel : public IGLTF2::IAnimationChannel, IArrayItem {
}; // end of struct IAnimationChannel

/** @hidden */
struct _IAnimationSamplerData {
  Float32Array input;
  IGLTF2::AnimationSamplerInterpolation interpolation;
  Float32Array output;
}; // end of struct _IAnimationSamplerData

/**
 * @brief Loader interface with additional members.
 */
struct IAnimationSampler : public IGLTF2::IAnimationSampler, IArrayItem {
  /** @hidden */
  std::optional<_IAnimationSamplerData> _data;
}; // end of struct IAnimationSampler

/**
 * @brief Loader interface with additional members.
 */
struct IAnimation : public IGLTF2::IAnimation, IArrayItem {
  /** @hidden */
  std::optional<AnimationGroupPtr> _babylonAnimationGroup;
}; // end of struct IAnimation

/**
 * @brief Loader interface with additional members.
 */
struct IBuffer : public IGLTF2::IBuffer, IArrayItem {
  /** @hidden */
  std::optional<ArrayBufferView> _data = std::nullopt;
}; // end of struct IBuffer

/**
 * @brief Loader interface with additional members.
 */
struct IBufferView : public IGLTF2::IBufferView, IArrayItem {
  /** @hidden */
  std::optional<ArrayBufferView> _data;

  /** @hidden */
  std::optional<Buffer> _babylonBuffer;
}; // end of struct IBufferView

/**
 * @brief Loader interface with additional members.
 */
struct ICamera : public IGLTF2::ICamera, IArrayItem {
}; // end of struct ICamera

/**
 * @brief Loader interface with additional members.
 */
struct IImage : public IGLTF2::IImage, IArrayItem {
  /** @hidden */
  std::optional<ArrayBufferView> _data;
}; // end of struct IImage

/**
 * @brief Loader interface with additional members.
 */
struct IMaterialNormalTextureInfo : public IGLTF2::IMaterialNormalTextureInfo {
}; // end of struct IMaterialNormalTextureInfo

/**
 * Loader interface with additional members.
 */
struct IMaterialOcclusionTextureInfo
    : public IGLTF2::IMaterialOcclusionTextureInfo {
}; // end of struct IMaterialOcclusionTextureInfo

/**
 * @brief Loader interface with additional members.
 */
struct IMaterialPbrMetallicRoughness
    : public IGLTF2::IMaterialPbrMetallicRoughness {
}; // end of struct IMaterialPbrMetallicRoughness

/**
 * @brief Loader interface with additional members.
 */
struct IMaterial : public IGLTF2::IMaterial, IArrayItem {
  struct IMaterialData {
    MaterialPtr babylonMaterial;
    std::vector<AbstractMeshPtr> babylonMeshes;
    std::optional<void> promise;
  }; // end of struct IMaterialData

  /** @hidden */
  // babylonDrawMode -> IMaterialData
  std::unordered_map<unsigned int, IMaterialData> _data;
}; // end of struct IMaterial

/**
 * @brief Loader interface with additional members.
 */
struct IMesh : public IGLTF2::IMesh, IArrayItem {
}; // end of struct IMesh

/**
 * @brief Loader interface with additional members.
 */
struct IMeshPrimitive : public IGLTF2::IMeshPrimitive, IArrayItem {
  struct IMeshPrimitiveData {
    MeshPtr babylonSourceMesh;
    std::optional<json> promise;
  }; // end of struct IMaterialData

  /** @hidden */
  std::optional<IMeshPrimitiveData> _instanceData = std::nullopt;
}; // end of struct IMeshPrimitive

/**
 * @brief Loader interface with additional members.
 */
struct INode : public IGLTF2::INode, IArrayItem {
  /**
   * The parent glTF node.
   */
  std::optional<INode> parent;

  /** @hidden */
  TransformNodePtr _babylonTransformNode = nullptr;

  /** @hidden */
  std::vector<AbstractMeshPtr> _primitiveBabylonMeshes;

  /** @hidden */
  std::vector<BonePtr> _babylonBones;

  /** @hidden */
  IndicesArray _numMorphTargets;
}; // end of struct INode

/** @hidden */
struct _ISamplerData {
  bool noMipMaps;
  unsigned int samplingMode;
  unsigned int wrapU;
  unsigned int wrapV;
}; // end of struct _ISamplerData

/**
 * @brief Loader interface with additional members.
 */
struct ISampler : public IGLTF2::ISampler, IArrayItem {
  /** @hidden */
  std::optional<_ISamplerData> _data = std::nullopt;
}; // end of struct ISampler

/**
 * @brief Loader interface with additional members.
 */
struct IScene : public IGLTF2::IScene, IArrayItem {
}; // end of struct IScene

/**
 * @brief Loader interface with additional members.
 */
struct ISkin : public IGLTF2::ISkin, IArrayItem {
  struct ISkinData {
    SkeletonPtr babylonSkeleton;
    std::optional<void> promise;
  }; // end of struct ISkinData

  /** @hidden */
  std::optional<ISkinData> _data = std::nullopt;
}; // end of struct ISkin

/**
 * @brief Loader interface with additional members.
 */
struct ITexture : public IGLTF2::ITexture, IArrayItem {
}; // end of struct ITexture

/**
 * @brief Loader interface with additional members.
 */
struct ITextureInfo : public IGLTF2::ITextureInfo {
}; // end of struct ITextureInfo

/**
 * @brief Loader interface with additional members.
 */
struct IGLTF : public IGLTF2::IGLTF {
}; // end of struct IGLTF

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_INTERFACES_H
