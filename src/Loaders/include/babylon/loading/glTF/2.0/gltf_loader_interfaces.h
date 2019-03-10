#ifndef BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_INTERFACES_H
#define BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_INTERFACES_H

#include <optional>

#include <nlohmann/json.hpp>

#include <babylon/core/array_buffer_view.h>
#include <babylon/mesh/vertex_buffer.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class AnimationGroup;
struct ArrayBufferView;
class Bone;
class Buffer;
class Material;
class Mesh;
class Skeleton;
class TransformNode;
class VertexBuffer;
using AbstractMeshPtr   = std::shared_ptr<AbstractMesh>;
using AnimationGroupPtr = std::shared_ptr<AnimationGroup>;
using BonePtr           = std::shared_ptr<Bone>;
using BufferPtr         = std::shared_ptr<Buffer>;
using MaterialPtr       = std::shared_ptr<Material>;
using MeshPtr           = std::shared_ptr<Mesh>;
using SkeletonPtr       = std::shared_ptr<Skeleton>;
using TransformNodePtr  = std::shared_ptr<TransformNode>;

namespace GLTF2 {

struct IAccessor;
struct IAnimation;
struct IAnimationChannel;
struct IAnimationSampler;
struct IBuffer;
struct IBufferView;
struct ICamera;
struct IImage;
struct IMaterial;
struct IMaterialNormalTextureInfo;
struct IMaterialOcclusionTextureInfo;
struct IMaterialPbrMetallicRoughness;
struct IMesh;
struct IMeshPrimitive;
struct INode;
struct ISampler;
struct IScene;
struct ISkin;
struct ITexture;
struct ITextureInfo;
using IMaterialNormalTextureInfoPtr
  = std::shared_ptr<IMaterialNormalTextureInfo>;
using IMaterialOcclusionTextureInfoPtr
  = std::shared_ptr<IMaterialOcclusionTextureInfo>;
using IMaterialPbrMetallicRoughnessPtr
  = std::shared_ptr<IMaterialPbrMetallicRoughness>;
using INodePtr        = std::shared_ptr<INode>;
using ITextureInfoPtr = std::shared_ptr<ITextureInfo>;

namespace IGLTF2 {

struct IAccessorSparse;
struct ICameraOrthographic;
struct ICameraPerspective;
using IAccessorSparsePtr     = std::shared_ptr<IAccessorSparse>;
using ICameraOrthographicPtr = std::shared_ptr<ICameraOrthographic>;
using ICameraPerspectivePtr  = std::shared_ptr<ICameraPerspective>;

/**
 * @brief Module for glTF 2.0 Interface.
 * @ignoreChildren
 * @ignore
 */

/**
 * @brief The datatype of the components in the attribute.
 */
enum class AccessorComponentType {
  /**
   * Invalid
   */
  INVALID = 0,
  /**
   * Byte
   */
  BYTE = 5120,
  /**
   * Unsigned Byte
   */
  UNSIGNED_BYTE = 5121,
  /**
   * Short
   */
  SHORT = 5122,
  /**
   * Unsigned Short
   */
  UNSIGNED_SHORT = 5123,
  /**
   * Unsigned Int
   */
  UNSIGNED_INT = 5125,
  /**
   * Float
   */
  FLOAT = 5126,
}; // end of enum class AccessorComponentType

/**
 * @brief Specifies if the attirbute is a scalar, vector, or matrix.
 */
enum class AccessorType {
  /**
   * Scalar
   */
  SCALAR,
  /**
   * Vector2
   */
  VEC2,
  /**
   * Vector3
   */
  VEC3,
  /**
   * Vector4
   */
  VEC4,
  /**
   * Matrix2x2
   */
  MAT2,
  /**
   * Matrix3x3
   */
  MAT3,
  /**
   * Matrix4x4
   */
  MAT4
}; // end enum class AccessorType

/**
 * @brief The name of the node's TRS property to modify, or the weights of the
 * Morph Targets it instantiates.
 */
enum class AnimationChannelTargetPath {
  /**
   * Invalid
   */
  INVALID = 0,
  /**
   * Translation
   */
  TRANSLATION,
  /**
   * Rotation
   */
  ROTATION,
  /**
   * Scale
   */
  SCALE,
  /**
   * Weights
   */
  WEIGHTS,
}; // end of enum class AnimationChannelTargetPath

/**
 * @brief Interpolation algorithm.
 */
enum class AnimationSamplerInterpolation {
  /**
   * Invalid
   */
  INVALID = 0,
  /**
   * The animated values are linearly interpolated between keyframes
   */
  LINEAR,
  /**
   * The animated values remain constant to the output of the first keyframe,
   * until the next keyframe
   */
  STEP,
  /**
   * The animation's interpolation is computed using a cubic spline with
   * specified tangents
   */
  CUBICSPLINE
}; // end of enum class AnimationSamplerInterpolation

/**
 * @brief A camera's projection.  A node can reference a camera to apply a
 * transform to place the camera in the scene.
 */
enum class CameraType {
  /**
   * Invalid
   */
  INVALID = 0,
  /**
   * A perspective camera containing properties to create a perspective
   * projection matrix
   */
  PERSPECTIVE,
  /**
   * An orthographic camera containing properties to create an orthographic
   * projection matrix
   */
  ORTHOGRAPHIC
}; // end of enum class CameraType

/**
 * @brief The mime-type of the image.
 */
enum class ImageMimeType {
  /**
   * JPEG Mime-type
   */
  JPEG,
  /**
   * PNG Mime-type
   */
  PNG
}; // end of enum class ImageMimeType

/**
 * @brief The alpha rendering mode of the material.
 */
enum class MaterialAlphaMode {
  /**
   * Invalid mode
   */
  INVALID,
  /**
   * The alpha value is ignored and the rendered output is fully opaque
   */
  OPAQUE,
  /**
   * The rendered output is either fully opaque or fully transparent depending
   * on the alpha value and the specified alpha cutoff value
   */
  MASK,
  /**
   * The alpha value is used to composite the source and destination areas. The
   * rendered output is combined with the background using the normal painting
   * operation (i.e. the Porter and Duff over operator)
   */
  BLEND,
}; // end of enum class MaterialAlphaMode

/**
 * @brief The type of the primitives to render.
 */
enum class MeshPrimitiveMode {
  /**
   * Points
   */
  POINTS = 0,
  /**
   * Lines
   */
  LINES = 1,
  /**
   * Line Loop
   */
  LINE_LOOP = 2,
  /**
   * Line Strip
   */
  LINE_STRIP = 3,
  /**
   * Triangles
   */
  TRIANGLES = 4,
  /**
   * Triangle Strip
   */
  TRIANGLE_STRIP = 5,
  /**
   * Triangle Fan
   */
  TRIANGLE_FAN = 6,
}; // end of class enum class MeshPrimitiveMode

/**
 * @brief Magnification filter.  Valid values correspond to WebGL enums: 9728
 * (NEAREST) and 9729 (LINEAR).
 */
enum class TextureMagFilter {
  /**
   * Nearest
   */
  NEAREST = 9728,
  /**
   * Linear
   */
  LINEAR = 9729,
}; // end of class enum class TextureMagFilter

/**
 * @brief Minification filter.  All valid values correspond to WebGL enums.
 */
enum class TextureMinFilter {
  /**
   * Invalid filter
   */
  INVALID = 0,
  /**
   * Nearest
   */
  NEAREST = 9728,
  /**
   * Linear
   */
  LINEAR = 9729,
  /**
   * Nearest Mip-Map Nearest
   */
  NEAREST_MIPMAP_NEAREST = 9984,
  /**
   * Linear Mipmap Nearest
   */
  LINEAR_MIPMAP_NEAREST = 9985,
  /**
   * Nearest Mipmap Linear
   */
  NEAREST_MIPMAP_LINEAR = 9986,
  /**
   * Linear Mipmap Linear
   */
  LINEAR_MIPMAP_LINEAR = 9987,
}; // end of enum class TextureMinFilter

/**
 * @brief S (U) wrapping mode.  All valid values correspond to WebGL enums.
 */
enum class TextureWrapMode {
  /**
   * Invalid mode
   */
  INVALID = 0,
  /**
   * Clamp to Edge
   */
  CLAMP_TO_EDGE = 33071,
  /**
   * Mirrored Repeat
   */
  MIRRORED_REPEAT = 33648,
  /**
   * Repeat
   */
  REPEAT = 10497,
}; // end of enum class TextureWrapMode

/**
 * @brief glTF Property
 */
struct IProperty {
  /**
   * Dictionary object with extension-specific objects
   */
  std::unordered_map<std::string, json> extensions;

  /**
   * Application-Specific data
   */
  json extras;
}; // end of struct IProperty

/**
 * @brief glTF Child of Root Property.
 */
struct IChildRootProperty : public IProperty {
  /**
   * The user-defined name of this object
   */
  std::string name;
}; // end of struct IChildRootProperty

/**
 * @brief Indices of those attributes that deviate from their initialization
 * value.
 */
struct IAccessorSparseIndices : public IProperty {
  /**
   * The index of the bufferView with sparse indices. Referenced bufferView
   * can't have ARRAY_BUFFER or ELEMENT_ARRAY_BUFFER target
   */
  size_t bufferView;
  /**
   * The offset relative to the start of the bufferView in bytes. Must be
   * aligned
   */
  std::optional<int> byteOffset = std::nullopt;
  /**
   * The indices data type.  Valid values correspond to WebGL enums: 5121
   * (UNSIGNED_BYTE), 5123 (UNSIGNED_SHORT), 5125 (UNSIGNED_INT)
   */
  AccessorComponentType componentType;
}; // end of struct IAccessorSparseIndices

/**
 * @brief Array of size accessor.sparse.count times number of components storing
 * the displaced accessor attributes pointed by accessor.sparse.indices.
 */
struct IAccessorSparseValues : public IProperty {
  /**
   * The index of the bufferView with sparse values. Referenced bufferView can't
   * have ARRAY_BUFFER or ELEMENT_ARRAY_BUFFER target
   */
  size_t bufferView;
  /**
   * The offset relative to the start of the bufferView in bytes. Must be
   * aligned
   */
  std::optional<int> byteOffset = std::nullopt;
}; // end of struct IAccessorSparseValues

/**
 * @brief Sparse storage of attributes that deviate from their initialization
 * value.
 */
struct IAccessorSparse : public IProperty {
  /**
   * The number of attributes encoded in this sparse accessor
   */
  size_t count;
  /**
   * Index array of size count that points to those accessor attributes that
   * deviate from their initialization value. Indices must strictly increase
   */
  IAccessorSparseIndices indices;
  /**
   * Array of size count times number of components, storing the displaced
   * accessor attributes pointed by indices. Substituted values must have the
   * same componentType and number of components as the base accessor
   */
  IAccessorSparseValues values;
}; // end of struct IAccessorSparse

/**
 * @brief A typed view into a bufferView.  A bufferView contains raw binary
 * data.  An accessor provides a typed view into a bufferView or a subset of a
 * bufferView similar to how WebGL's vertexAttribPointer() defines an attribute
 * in a buffer.
 */
struct IAccessor : public IChildRootProperty {
  /**
   * The index of the bufferview
   */
  std::optional<size_t> bufferView = std::nullopt;
  /**
   * The offset relative to the start of the bufferView in bytes
   */
  std::optional<size_t> byteOffset = std::nullopt;
  /**
   * The datatype of components in the attribute
   */
  AccessorComponentType componentType;
  /**
   * Specifies whether integer data values should be normalized
   */
  std::optional<bool> normalized = std::nullopt;
  /**
   * The number of attributes referenced by this accessor
   */
  size_t count;
  /**
   * Specifies if the attribute is a scalar, vector, or matrix
   */
  AccessorType type;
  /**
   * Maximum value of each component in this attribute
   */
  Int32Array max;
  /**
   * Minimum value of each component in this attribute
   */
  Int32Array min;
  /**
   * Sparse storage of attributes that deviate from their initialization value
   */
  IAccessorSparsePtr sparse;
}; // end of struct IAccessor

/**
 * @brief The index of the node and TRS property that an animation channel
 * targets.
 */
struct IAnimationChannelTarget : public IProperty {
  /**
   * The index of the node to target
   */
  std::optional<size_t> node = std::nullopt;
  /**
   * The name of the node's TRS property to modify, or the weights of the Morph
   * Targets it instantiates
   */
  AnimationChannelTargetPath path;
};

/**
 * @brief Targets an animation's sampler at a node's property.
 */
struct IAnimationChannel : public IProperty {
  /**
   * The index of a sampler in this animation used to compute the value for the
   * target
   */
  size_t sampler;
  /**
   * The index of the node and TRS property to target
   */
  IAnimationChannelTarget target;
}; // end of struct IAnimationChannel

/**
 * @brief Combines input and output accessors with an interpolation algorithm to
 * define a keyframe graph (but not its target).
 */
struct IAnimationSampler : public IProperty {
  /**
   * The index of an accessor containing keyframe input values, e.g., time
   */
  size_t input;
  /**
   * Interpolation algorithm
   */
  std::optional<AnimationSamplerInterpolation> interpolation = std::nullopt;
  /**
   * The index of an accessor, containing keyframe output values
   */
  size_t output;
}; // end of struct IAnimationSampler

/**
 * @brief A keyframe animation.
 */
struct IAnimation : public IChildRootProperty {
  /**
   * An array of channels, each of which targets an animation's sampler at a
   * node's property
   */
  std::vector<GLTF2::IAnimationChannel> channels;
  /**
   * An array of samplers that combines input and output accessors with an
   * interpolation algorithm to define a keyframe graph (but not its target)
   */
  std::vector<GLTF2::IAnimationSampler> samplers;
}; // end of struct IAnimation

/**
 * @brief Metadata about the glTF asset.
 */
struct IAsset : public IChildRootProperty {
  /**
   * A copyright message suitable for display to credit the content creator
   */
  std::string copyright;
  /**
   * Tool that generated this glTF model.  Useful for debugging
   */
  std::string generator;
  /**
   * The glTF version that this asset targets
   */
  std::string version;
  /**
   * The minimum glTF version that this asset targets
   */
  std::string minVersion;
}; // end of struct IAsset

/**
 * @brief A buffer points to binary geometry, animation, or skins.
 */
struct IBuffer : public IChildRootProperty {
  /**
   * The uri of the buffer.  Relative paths are relative to the .gltf file.
   * Instead of referencing an external file, the uri can also be a data-uri
   */
  std::string uri;
  /**
   * The length of the buffer in bytes
   */
  size_t byteLength;
}; // end of struct IBuffer

/**
 * @brief A view into a buffer generally representing a subset of the buffer.
 */
struct IBufferView : public IChildRootProperty {
  /**
   * The index of the buffer
   */
  size_t buffer;
  /**
   * The offset into the buffer in bytes
   */
  std::optional<int> byteOffset = std::nullopt;
  /**
   * The lenth of the bufferView in bytes
   */
  size_t byteLength;
  /**
   * The stride, in bytes
   */
  std::optional<size_t> byteStride = std::nullopt;
}; // end of struct IBufferView

/**
 * @brief An orthographic camera containing properties to create an orthographic
 * projection matrix.
 */
struct ICameraOrthographic : public IProperty {
  /**
   * The floating-point horizontal magnification of the view. Must not be zero
   */
  float xmag;
  /**
   * The floating-point vertical magnification of the view. Must not be zero
   */
  float ymag;
  /**
   * The floating-point distance to the far clipping plane. zfar must be greater
   * than znear
   */
  float zfar;
  /**
   * The floating-point distance to the near clipping plane
   */
  float znear;
}; // end of struct ICameraOrthographic

/**
 * @brief A perspective camera containing properties to create a perspective
 * projection matrix.
 */
struct ICameraPerspective : public IProperty {
  /**
   * The floating-point aspect ratio of the field of view
   */
  std::optional<float> aspectRatio = std::nullopt;
  /**
   * The floating-point vertical field of view in radians
   */
  float yfov;
  /**
   * The floating-point distance to the far clipping plane
   */
  std::optional<float> zfar = std::nullopt;
  /**
   * The floating-point distance to the near clipping plane
   */
  float znear;
}; // end of struct IProperty

/**
 * @brief A camera's projection.  A node can reference a camera to apply a
 * transform to place the camera in the scene.
 */
struct ICamera : public IChildRootProperty {
  /**
   * An orthographic camera containing properties to create an orthographic
   * projection matrix
   */
  ICameraOrthographicPtr orthographic = nullptr;
  /**
   * A perspective camera containing properties to create a perspective
   * projection matrix
   */
  ICameraPerspectivePtr perspective = nullptr;
  /**
   * Specifies if the camera uses a perspective or orthographic projection
   */
  CameraType type;
}; // end of struct ICamera

/**
 * @brief Image data used to create a texture. Image can be referenced by URI or
 * bufferView index. mimeType is required in the latter case.
 */
struct IImage : public IChildRootProperty {
  /**
   * The uri of the image.  Relative paths are relative to the .gltf file.
   * Instead of referencing an external file, the uri can also be a data-uri.
   * The image format must be jpg or png
   */
  std::string uri;
  /**
   * The image's MIME type
   */
  std::optional<ImageMimeType> mimeType = std::nullopt;
  /**
   * The index of the bufferView that contains the image. Use this instead of
   * the image's uri property
   */
  std::optional<size_t> bufferView = std::nullopt;
}; // end of struct IImage

/**
 * @brief Reference to a texture.
 */
struct ITextureInfo : public IProperty {
  /**
   * The index of the texture
   */
  size_t index;
  /**
   * The set index of texture's TEXCOORD attribute used for texture coordinate
   * mapping
   */
  std::optional<unsigned int> texCoord = std::nullopt;
}; // end of struct ITextureInfo

/**
 * @brief Material Normal Texture Info.
 */
struct IMaterialNormalTextureInfo : public ITextureInfo {
  /**
   * The scalar multiplier applied to each normal vector of the normal texture
   */
  std::optional<float> scale = std::nullopt;
}; // end of struct IMaterialNormalTextureInfo

/**
 * @brief Material Occlusion Texture Info.
 */
struct IMaterialOcclusionTextureInfo : public ITextureInfo {
  /**
   * A scalar multiplier controlling the amount of occlusion applied
   */
  std::optional<float> strength = std::nullopt;
}; // end of struct IMaterialOcclusionTextureInfo

/**
 * @brief A set of parameter values that are used to define the
 * metallic-roughness material model from Physically-Based Rendering (PBR)
 * methodology.
 */
struct IMaterialPbrMetallicRoughness {
  /**
   * The material's base color factor
   */
  Float32Array baseColorFactor;
  /**
   * The base color texture
   */
  ITextureInfoPtr baseColorTexture = nullptr;
  /**
   * The metalness of the material
   */
  std::optional<float> metallicFactor = std::nullopt;
  /**
   * The roughness of the material
   */
  std::optional<float> roughnessFactor = std::nullopt;
  /**
   * The metallic-roughness texture
   */
  ITextureInfoPtr metallicRoughnessTexture = nullptr;
}; // end of struct IMaterialPbrMetallicRoughness

/**
 * @brief The material appearance of a primitive.
 */
struct IMaterial : public IChildRootProperty {
  /**
   * A set of parameter values that are used to define the metallic-roughness
   * material model from Physically-Based Rendering (PBR) methodology. When not
   * specified, all the default values of pbrMetallicRoughness apply
   */
  IMaterialPbrMetallicRoughnessPtr pbrMetallicRoughness = nullptr;
  /**
   * The normal map texture
   */
  IMaterialNormalTextureInfoPtr normalTexture = nullptr;
  /**
   * The occlusion map texture
   */
  IMaterialOcclusionTextureInfoPtr occlusionTexture = nullptr;
  /**
   * The emissive map texture
   */
  ITextureInfoPtr emissiveTexture = nullptr;
  /**
   * The RGB components of the emissive color of the material. These values are
   * linear. If an emissiveTexture is specified, this value is multiplied with
   * the texel values
   */
  Float32Array emissiveFactor;
  /**
   * The alpha rendering mode of the material
   */
  std::optional<MaterialAlphaMode> alphaMode = std::nullopt;
  /**
   * The alpha cutoff value of the material
   */
  std::optional<float> alphaCutoff = std::nullopt;
  /**
   * Specifies whether the material is double sided
   */
  std::optional<bool> doubleSided = std::nullopt;
}; // end of struct IMaterial

/**
 * @brief Geometry to be rendered with the given material.
 */
struct IMeshPrimitive : public IProperty {
  /**
   * A dictionary object, where each key corresponds to mesh attribute semantic
   * and each value is the index of the accessor containing attribute's data
   */
  std::unordered_map<std::string, size_t> attributes;

  /**
   * The index of the accessor that contains the indices
   */
  std::optional<size_t> indices = std::nullopt;
  /**
   * The index of the material to apply to this primitive when rendering
   */
  std::optional<size_t> material = std::nullopt;
  /**
   * The type of primitives to render. All valid values correspond to WebGL
   * enums
   */
  std::optional<MeshPrimitiveMode> mode = std::nullopt;
  /**
   * An array of Morph Targets, each  Morph Target is a dictionary mapping
   * attributes (only POSITION, NORMAL, and TANGENT supported) to their
   * deviations in the Morph Target
   */
  std::vector<std::unordered_map<std::string, size_t>> targets;
}; // end of struct IMeshPrimitive

/**
 * @brief A set of primitives to be rendered.  A node can contain one mesh.  A
 * node's transform places the mesh in the scene.
 */
struct IMesh : public IChildRootProperty {
  /**
   * An array of primitives, each defining geometry to be rendered with a
   * material
   */
  std::vector<GLTF2::IMeshPrimitive> primitives;
  /**
   * Array of weights to be applied to the Morph Targets
   */
  Float32Array weights;
}; // end of  struct IMesh

/**
 * @brief A node in the node hierarchy.
 */
struct INode : public IChildRootProperty {
  /**
   * The index of the camera referenced by this node
   */
  std::optional<size_t> camera = std::nullopt;
  /**
   * The indices of this node's children
   */
  IndicesArray children;
  /**
   * The index of the skin referenced by this node
   */
  std::optional<size_t> skin = std::nullopt;
  /**
   * A floating-point 4x4 transformation matrix stored in column-major order
   */
  Float32Array matrix;
  /**
   * The index of the mesh in this node
   */
  std::optional<size_t> mesh = std::nullopt;
  /**
   * The node's unit quaternion rotation in the order (x, y, z, w), where w is
   * the scalar
   */
  Float32Array rotation;
  /**
   * The node's non-uniform scale, given as the scaling factors along the x, y,
   * and z axes
   */
  Float32Array scale;
  /**
   * The node's translation along the x, y, and z axes
   */
  Float32Array translation;
  /**
   * The weights of the instantiated Morph Target. Number of elements must match
   * number of Morph Targets of used mesh
   */
  Float32Array weights;
}; // end of struct INode

/**
 * @brief Texture sampler properties for filtering and wrapping modes.
 */
struct ISampler : public IChildRootProperty {
  /**
   * Magnification filter.  Valid values correspond to WebGL enums: 9728
   * (NEAREST) and 9729 (LINEAR)
   */
  std::optional<TextureMagFilter> magFilter = std::nullopt;
  /**
   * Minification filter.  All valid values correspond to WebGL enums
   */
  std::optional<TextureMinFilter> minFilter = std::nullopt;
  /**
   * S (U) wrapping mode.  All valid values correspond to WebGL enums
   */
  std::optional<TextureWrapMode> wrapS = std::nullopt;
  /**
   * T (V) wrapping mode.  All valid values correspond to WebGL enums
   */
  std::optional<TextureWrapMode> wrapT = std::nullopt;
}; // end of struct ISampler

/**
 * @brief The root nodes of a scene.
 */
struct IScene : public IChildRootProperty {
  /**
   * The indices of each root node
   */
  std::vector<size_t> nodes;
}; // end of struct IScene

/**
 * @brief Joints and matrices defining a skin.
 */
struct ISkin : public IChildRootProperty {
  /**
   * The index of the accessor containing the floating-point 4x4 inverse-bind
   * matrices.  The default is that each matrix is a 4x4 identity matrix, which
   * implies that inverse-bind matrices were pre-applied
   */
  std::optional<size_t> inverseBindMatrices = std::nullopt;
  /**
   * The index of the node used as a skeleton root. When undefined, joints
   * transforms resolve to scene root
   */
  std::optional<size_t> skeleton = std::nullopt;
  /**
   * Indices of skeleton nodes, used as joints in this skin.  The array length
   * must be the same as the count property of the inverseBindMatrices accessor
   * (when defined)
   */
  IndicesArray joints;
}; // end of struct ISkin

/**
 * @brief A texture and its sampler.
 */
struct ITexture : public IChildRootProperty {
  /**
   * The index of the sampler used by this texture. When undefined, a sampler
   * with repeat wrapping and auto filtering should be used
   */
  std::optional<size_t> sampler = std::nullopt;
  /**
   * The index of the image used by this texture
   */
  size_t source;
}; // end of struct ITexture

/**
 * @brief The root object for a glTF asset.
 */
struct IGLTF : public IProperty {
  /**
   * An array of accessors. An accessor is a typed view into a bufferView
   */
  std::vector<GLTF2::IAccessor> accessors;
  /**
   * An array of keyframe animations
   */
  std::vector<GLTF2::IAnimation> animations;
  /**
   * Metadata about the glTF asset
   */
  IAsset asset;
  /**
   * An array of buffers.  A buffer points to binary geometry, animation, or
   * skins
   */
  std::vector<GLTF2::IBuffer> buffers;
  /**
   * An array of bufferViews.  A bufferView is a view into a buffer generally
   * representing a subset of the buffer
   */
  std::vector<GLTF2::IBufferView> bufferViews;
  /**
   * An array of cameras
   */
  std::vector<GLTF2::ICamera> cameras;
  /**
   * Names of glTF extensions used somewhere in this asset
   */
  std::vector<std::string> extensionsUsed;
  /**
   * Names of glTF extensions required to properly load this asset
   */
  std::vector<std::string> extensionsRequired;
  /**
   * An array of images.  An image defines data used to create a texture
   */
  std::vector<GLTF2::IImage> images;
  /**
   * An array of materials.  A material defines the appearance of a primitive
   */
  std::vector<GLTF2::IMaterial> materials;
  /**
   * An array of meshes.  A mesh is a set of primitives to be rendered
   */
  std::vector<GLTF2::IMesh> meshes;
  /**
   * An array of nodes
   */
  std::vector<INodePtr> nodes;
  /**
   * An array of samplers.  A sampler contains properties for texture filtering
   * and wrapping modes
   */
  std::vector<GLTF2::ISampler> samplers;
  /**
   * The index of the default scene
   */
  std::optional<size_t> scene = std::nullopt;
  /**
   * An array of scenes
   */
  std::vector<GLTF2::IScene> scenes;
  /**
   * An array of skins.  A skin is defined by joints and matrices
   */
  std::vector<GLTF2::ISkin> skins;
  /**
   * An array of textures
   */
  std::vector<GLTF2::ITexture> textures;
}; // end of struct struct IGLTF

/**
 * @brief Interface for glTF validation results.
 */
struct IGLTFValidationResults {
  struct info {
    std::string generator;
    bool hasAnimations;
    bool hasDefaultScene;
    bool hasMaterials;
    bool hasMorphTargets;
    bool hasSkins;
    bool hasTextures;
    size_t maxAttributesUsed;
    size_t primitivesCount;
  }; // end of struct info
  struct issues {
    std::vector<std::string> messages;
    size_t numErrors;
    size_t numHints;
    size_t numInfos;
    size_t numWarnings;
    bool truncated;
  }; // end of struct issues
  std::string mimeType;
  std::string uri;
  std::string validatedAt;
  std::string validatorVersion;
}; // end of struct IGLTFValidationResults;

/**
 * @brief Interface for glTF validation options.
 */
struct IGLTFValidationOptions {
  std::string uri;
  std::function<Uint8Array(const std::string& uri)> externalResourceFunction
    = nullptr;
  std::optional<bool> validateAccessorData = std::nullopt;
  std::optional<size_t> maxIssues          = std::nullopt;
  std::vector<std::string> ignoredIssues;
  json severityOverrides;
}; // end of struct IGLTFValidationOptions

/**
 * @brief Interface validator object Typings.
 */
struct IGLTFValidatorTypings {
  std::function<IGLTFValidationResults(
    const std::string& json, std::optional<IGLTFValidationOptions>& options)>
    externalResourceFunction = nullptr;
}; // end of struct IGLTFValidatorTypings

} // end namespace IGLTF2

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
  std::optional<ArrayBufferView> _data = std::nullopt;

  /** @hidden */
  std::unique_ptr<VertexBuffer> _babylonVertexBuffer = nullptr;
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
  std::optional<_IAnimationSamplerData> _data = std::nullopt;
}; // end of struct IAnimationSampler

/**
 * @brief Loader interface with additional members.
 */
struct IAnimation : public IGLTF2::IAnimation, IArrayItem {
  /** @hidden */
  AnimationGroupPtr _babylonAnimationGroup = nullptr;
}; // end of struct IAnimation

/**
 * @brief Loader interface with additional members.
 */
struct IBuffer : public IGLTF2::IBuffer, IArrayItem {
  /** @hidden */
  ArrayBufferView _data;
}; // end of struct IBuffer

/**
 * @brief Loader interface with additional members.
 */
struct IBufferView : public IGLTF2::IBufferView, IArrayItem {
  /** @hidden */
  ArrayBufferView _data;

  /** @hidden */
  BufferPtr _babylonBuffer = nullptr;
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
  ArrayBufferView _data;
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

struct IMaterialData {
  MaterialPtr babylonMaterial;
  std::vector<AbstractMeshPtr> babylonMeshes;
  std::function<void()> promise = nullptr;
}; // end of struct IMaterialData

/**
 * @brief Loader interface with additional members.
 */
struct IMaterial : public IGLTF2::IMaterial, IArrayItem {
  /** @hidden */
  // babylonDrawMode -> IMaterialData
  std::unordered_map<unsigned int, GLTF2::IMaterialData> _data;
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
    MeshPtr babylonSourceMesh   = nullptr;
    std::optional<json> promise = std::nullopt;
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
  INode* parent = nullptr;

  /** @hidden */
  TransformNodePtr _babylonTransformNode = nullptr;

  /** @hidden */
  std::vector<AbstractMeshPtr> _primitiveBabylonMeshes;

  /** @hidden */
  std::vector<BonePtr> _babylonBones;

  /** @hidden */
  std::optional<size_t> _numMorphTargets = std::nullopt;
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
    SkeletonPtr babylonSkeleton = nullptr;
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
