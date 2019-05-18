#ifndef BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H
#define BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H

#include <memory>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/engines/engine_constants.h>
#include <babylon/math/matrix.h>

namespace BABYLON {

class AbstractMesh;
class Effect;
class Mesh;
class SubMesh;
class MultiRenderTarget;
class Scene;
class SubMesh;
using AbstractMeshPtr      = std::shared_ptr<AbstractMesh>;
using EffectPtr            = std::shared_ptr<Effect>;
using MeshPtr              = std::shared_ptr<Mesh>;
using SubMeshPtr           = std::shared_ptr<SubMesh>;
using MultiRenderTargetPtr = std::shared_ptr<MultiRenderTarget>;

struct ISavedTransformationMatrix {
  Matrix world;
  Matrix viewProjection;
}; // end of struct ISavedTransformationMatrix

/**
 * @brief This renderer is helpfull to fill one of the render target with a
 * geometry buffer.
 */
class BABYLON_SHARED_EXPORT GeometryBufferRenderer {

public:
  /**
   * Constant used to retrieve the position texture index in the G-Buffer
   * textures array using
   * getIndex(GeometryBufferRenderer.POSITION_TEXTURE_INDEX)
   */
  static constexpr unsigned int POSITION_TEXTURE_TYPE = 1;
  /**
   * Constant used to retrieve the velocity texture index in the G-Buffer
   * textures array using
   * getIndex(GeometryBufferRenderer.VELOCITY_TEXTURE_INDEX)
   */
  static constexpr unsigned int VELOCITY_TEXTURE_TYPE = 2;

public:
  /**
   * @brief Creates a new G Buffer for the scene.
   * @param scene The scene the buffer belongs to
   * @param ratio How big is the buffer related to the main canvas.
   */
  GeometryBufferRenderer(Scene* scene, float ratio = 1.f);
  virtual ~GeometryBufferRenderer();

  /**
   * @brief Returns the index of the given texture type in the G-Buffer textures
   * array.
   * @param textureType The texture type constant. For example
   * GeometryBufferRenderer.POSITION_TEXTURE_INDEX
   * @returns the index of the given texture type in the G-Buffer textures array
   */
  int getTextureIndex(unsigned int textureType);

  /**
   * @brief Checks wether everything is ready to render a submesh to the G
   * buffer.
   * @param subMesh the submesh to check readiness for
   * @param useInstances is the mesh drawn using instance or not
   * @returns true if ready otherwise false
   */
  bool isReady(SubMesh* subMesh, bool useInstances);

  /**
   * @brief Gets the current underlying G Buffer.
   * @returns the buffer
   */
  MultiRenderTargetPtr getGBuffer() const;

  /**
   * @brief Disposes the renderer and frees up associated resources.
   */
  void dispose();

protected:
  /**
   * @brief Set the render list (meshes to be rendered) used in the G buffer.
   */
  void set_renderList(const std::vector<MeshPtr>& meshes);

  /**
   * @brief Gets wether or not G buffer are supported by the running hardware.
   * This requires draw buffer supports
   */
  bool get_isSupported() const;

  /**
   * @brief Gets a boolean indicating if objects positions are enabled for the G
   * buffer.
   */
  bool get_enablePosition() const;

  /**
   * @brief Sets whether or not objects positions are enabled for the G buffer.
   */
  void set_enablePosition(bool enable);

  /**
   * @brief Gets a boolean indicating if objects velocities are enabled for the
   * G buffer.
   */
  bool get_enableVelocity() const;

  /**
   * @brief Sets wether or not objects velocities are enabled for the G buffer.
   */
  void set_enableVelocity(bool enable);

  /**
   * @brief Gets the scene associated with the buffer.
   */
  Scene*& get_scene();

  /**
   * @brief Gets the ratio used by the buffer during its creation.
   * How big is the buffer related to the main canvas.
   */
  float get_ratio() const;

  /**
   * @brief Gets the number of samples used to render the buffer (anti
   * aliasing).
   */
  unsigned int get_samples() const;

  /**
   * @brief Sets the number of samples used to render the buffer (anti
   * aliasing).
   */
  void set_samples(unsigned int value);

  void _createRenderTargets();

private:
  /**
   * @brief Custom render function.
   * @param subMesh
   */
  void renderSubMesh(SubMesh* subMesh);

  /**
   * @brief _Copies the bones transformation matrices into the target array and
   * returns the target's reference.
   */
  Float32Array& _copyBonesTransformationMatrices(const Float32Array& source,
                                                 Float32Array& target);

public:
  /**
   * Dictionary used to store the previous transformation matrices of each
   * rendered mesh in order to compute objects velocities when enableVelocity is
   * set to "true"
   * @hidden
   */
  std::unordered_map<size_t, ISavedTransformationMatrix>
    _previousTransformationMatrices;

  /**
   * Dictionary used to store the previous bones transformation matrices of each
   * rendered mesh in order to compute objects velocities when enableVelocity is
   * set to "true"
   * @hidden
   */

  std::unordered_map<size_t, Float32Array> _previousBonesTransformationMatrices;

  /**
   * Array used to store the ignored skinned meshes while computing velocity map
   * (typically used by the motion blur post-process). Avoids computing bones
   * velocities and computes only mesh's velocity itself (position, rotation,
   * scaling).
   */
  std::vector<AbstractMeshPtr> excludedSkinnedMeshesFromVelocity;

  /**
   * The render list (meshes to be rendered) used in the G buffer.
   */
  WriteOnlyProperty<GeometryBufferRenderer, std::vector<MeshPtr>> renderList;

  /**
   * Wether or not G buffer are supported by the running hardware.
   * This requires draw buffer supports
   */
  ReadOnlyProperty<GeometryBufferRenderer, bool> isSupported;

  /**
   * Wether or not position are enabled for the G buffer.
   */
  Property<GeometryBufferRenderer, bool> enablePosition;

  /**
   * Wether or not objects velocities are enabled for the G buffer.
   */
  Property<GeometryBufferRenderer, bool> enableVelocity;

  /**
   * The scene associated with the buffer.
   */
  ReadOnlyProperty<GeometryBufferRenderer, Scene*> scene;

  /**
   * The ratio used by the buffer during its creation.
   * How big is the buffer related to the main canvas.
   */
  ReadOnlyProperty<GeometryBufferRenderer, float> ratio;

  /**
   * The number of samples used to render the buffer (anti aliasing).
   */
  Property<GeometryBufferRenderer, unsigned int> samples;

protected:
  EffectPtr _effect;
  std::string _cachedDefines;

private:
  Scene* _scene;
  MultiRenderTargetPtr _multiRenderTarget;
  float _ratio;
  bool _enablePosition;
  bool _enableVelocity;

  int _positionIndex;
  int _velocityIndex;

}; // end of class GeometryBufferRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H
