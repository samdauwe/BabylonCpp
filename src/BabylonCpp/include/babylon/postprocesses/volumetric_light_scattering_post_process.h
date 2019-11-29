#ifndef BABYLON_POSTPROCESSES_VOLUMETRIC_LIGHT_SCATTERING_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_VOLUMETRIC_LIGHT_SCATTERING_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/maths/viewport.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class AbstractMesh;
class Mesh;
class RenderTargetTexture;
class SubMesh;
class VolumetricLightScatteringPostProcess;
using AbstractMeshPtr        = std::shared_ptr<AbstractMesh>;
using MeshPtr                = std::shared_ptr<Mesh>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;
using SubMeshPtr             = std::shared_ptr<SubMesh>;
using VolumetricLightScatteringPostProcessPtr
  = std::shared_ptr<VolumetricLightScatteringPostProcess>;

/**
 * @brief VolumetricLightScatteringPostProcess class.
 *
 * Inspired by http://http.developer.nvidia.com/GPUGems3/gpugems3_ch13.html
 */
class BABYLON_SHARED_EXPORT VolumetricLightScatteringPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static VolumetricLightScatteringPostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<VolumetricLightScatteringPostProcess>(
      new VolumetricLightScatteringPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~VolumetricLightScatteringPostProcess() override; // = default

  /**
   * @brief Returns the string "VolumetricLightScatteringPostProcess".
   * @returns "VolumetricLightScatteringPostProcess"
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Sets the new light position for light scattering effect.
   * @param position The new custom light position
   */
  void setCustomMeshPosition(const Vector3& position);

  /**
   * @brief Returns the light position for light scattering effect.
   * @return Vector3 The custom light position
   */
  Vector3& getCustomMeshPosition();

  /**
   * @brief Disposes the internal assets and detaches the post-process from the
   * camera.
   */
  void dispose(Camera* camera) override;

  /**
   * @brief Returns the render target texture used by the post-process.
   * @return the render target texture used by the post-process
   */
  RenderTargetTexturePtr& getPass();

  //** Static methods **/ /
  /**
   * @brief Creates a default mesh for the Volumeric Light Scattering
   * post-process.
   * @param name The mesh name
   * @param scene The scene where to create the mesh
   * @return the default mesh
   */
  static MeshPtr CreateDefaultMesh(const std::string& name, Scene* scene);

protected:
  /**
   * @brief Constructor
   * @param name The post-process name
   * @param ratio The size of the post-process and/or internal pass (0.5 means
   * that your postprocess will have a width = canvas.width 0.5 and a height =
   * canvas.height 0.5)
   * @param camera The camera that the post-process will be attached to
   * @param mesh The mesh used to create the light scattering
   * @param samples The post-process quality, default 100
   * @param samplingModeThe post-process filtering mode
   * @param engine The babylon engine
   * @param reusable If the post-process is reusable
   * @param scene The constructor needs a scene reference to initialize internal
   * components. If "camera" is null a "scene" must be provided
   */
  VolumetricLightScatteringPostProcess(
    const std::string& name, float ratio, const CameraPtr& camera, const MeshPtr& mesh,
    unsigned int samples = 100, unsigned int samplingMode = TextureConstants::BILINEAR_SAMPLINGMODE,
    Engine* engine = nullptr, bool reusable = false, Scene* scene = nullptr);

private:
  bool _isReady(SubMesh* subMesh, bool useInstances);
  bool _meshExcluded(const AbstractMeshPtr& mesh);
  void _createPass(Scene* scene, float ratio);
  void _updateMeshScreenCoordinates(Scene* scene);

public:
  /**
   * If not undefined, the mesh position is computed from the attached node
   * position
   */
  Vector3* attachedNode;
  /**
   * Custom position of the mesh. Used if "useCustomMeshPosition" is set to
   * "true"
   */
  Vector3 customMeshPosition;
  /**
   * Set if the post-process should use a custom position for the light source
   * (true) or the internal mesh position (false)
   */
  bool useCustomMeshPosition;
  /**
   * If the post-process should inverse the light scattering direction
   */
  bool invert;
  /**
   * The internal mesh used by the post-process
   */
  MeshPtr mesh;
  /**
   * Set to true to use the diffuseColor instead of the diffuseTexture
   */
  bool useDiffuseColor;
  /**
   * Array containing the excluded meshes not rendered in the internal pass
   */
  std::vector<AbstractMeshPtr> excludedMeshes;
  /**
   * Controls the overall intensity of the post-process
   */
  float exposure;
  /**
   * Dissipates each sample's contribution in range [0, 1]
   */
  float decay;
  /**
   * Controls the overall intensity of each sample
   */
  float weight;
  /**
   * Controls the density of each sample
   */
  float density;

private:
  EffectPtr _volumetricLightScatteringPass;
  RenderTargetTexturePtr _volumetricLightScatteringRTT;
  Viewport _viewPort;
  Vector2 _screenCoordinates;
  std::string _cachedDefines;
  Vector3 _customMeshPosition;

}; // end of class VolumetricLightScatteringPostProcess

} // end of namespace BABYLON

#endif // end of
       // BABYLON_POSTPROCESSES_VOLUMETRIC_LIGHT_SCATTERING_POST_PROCESS_H
