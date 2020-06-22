#ifndef BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/water/water_material_defines.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/plane.h>
#include <babylon/maths/vector2.h>

namespace BABYLON {

class AbstractMesh;
class IAnimatable;
class ImageProcessingConfiguration;
class Mesh;
using AbstractMeshPtr                 = std::shared_ptr<AbstractMesh>;
using IAnimatablePtr                  = std::shared_ptr<IAnimatable>;
using ImageProcessingConfigurationPtr = std::shared_ptr<ImageProcessingConfiguration>;
using MeshPtr                         = std::shared_ptr<Mesh>;

namespace MaterialsLibrary {

class WaterMaterial;
using WaterMaterialPtr = std::shared_ptr<WaterMaterial>;

class BABYLON_SHARED_EXPORT WaterMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static WaterMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<WaterMaterial>(new WaterMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~WaterMaterial() override; // = default

  // Methods
  void addToRenderList(const AbstractMeshPtr& node);
  void enableRenderTargets(bool enable);
  std::vector<AbstractMesh*>& getRenderList();
  [[nodiscard]] bool needAlphaBlending() const override;
  [[nodiscard]] bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances = false) override;
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables() override;
  [[nodiscard]] std::vector<BaseTexturePtr> getActiveTextures() const override;
  [[nodiscard]] bool hasTexture(const BaseTexturePtr& texture) const override;
  [[nodiscard]] std::string getClassName() const override;
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool notBoundToMesh = false) override;
  [[nodiscard]] MaterialPtr clone(const std::string& name,
                                  bool cloneChildren = false) const override;
  [[nodiscard]] json serialize() const;

  /** Statics **/
  static WaterMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);
  static MeshPtr CreateDefaultMesh(const std::string& name, Scene* scene);

protected:
  WaterMaterial(const std::string& name, Scene* scene,
                const Vector2& renderTargetSize = Vector2(512.f, 512.f));

  BaseTexturePtr& get_bumpTexture();
  void set_bumpTexture(const BaseTexturePtr& value);
  [[nodiscard]] bool get_disableLighting() const;
  void set_disableLighting(bool value);
  [[nodiscard]] unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);
  [[nodiscard]] bool get_bumpSuperimpose() const;
  void set_bumpSuperimpose(bool value);
  [[nodiscard]] bool get_fresnelSeparate() const;
  void set_fresnelSeparate(bool value);
  [[nodiscard]] bool get_bumpAffectsReflection() const;
  void set_bumpAffectsReflection(bool value);
  [[nodiscard]] bool get_useLogarithmicDepth() const override;
  void set_useLogarithmicDepth(bool value) override;

  /**
   * @brief Gets a boolean indicating that current material needs to register
   * RTT.
   */
  [[nodiscard]] bool get_hasRenderTargetTextures() const override;

  RenderTargetTexturePtr& get_refractionTexture();
  RenderTargetTexturePtr& get_reflectionTexture();
  [[nodiscard]] bool get_renderTargetsEnabled() const;
  float get_lastTime() const;

private:
  void _createRenderTargets(Scene* scene, const Vector2& renderTargetSize);

public:
  /*
   * Public members
   */
  Property<WaterMaterial, BaseTexturePtr> bumpTexture;
  Color3 diffuseColor;
  Color3 specularColor;
  float specularPower;
  Property<WaterMaterial, bool> disableLighting;
  Property<WaterMaterial, unsigned int> maxSimultaneousLights;
  /**
   * Represents the wind force
   */
  float windForce;
  /**
   * The direction of the wind in the plane (X, Z)
   */
  Vector2 windDirection;
  /**
   * Wave height, represents the height of the waves
   */
  float waveHeight;
  /**
   * Bump height, represents the bump height related to the
   * bump map
   */
  float bumpHeight;
  /**
   * Add a smaller moving bump to less steady waves
   */
  Property<WaterMaterial, bool> bumpSuperimpose;
  /**
   * Color refraction and reflection differently with .waterColor2 and .colorBlendFactor2.
   * Non-linear (physically correct) fresnel
   */
  Property<WaterMaterial, bool> fresnelSeparate;
  /**
   * Bump Waves modify the reflection
   */
  Property<WaterMaterial, bool> bumpAffectsReflection;
  /**
   * The water color blended with the reflection and refraction samplers
   */
  Color3 waterColor;
  /**
   * The blend factor related to the water color
   */
  float colorBlendFactor;
  /**
   * The water color blended with the reflection (far)
   */
  Color3 waterColor2;
  /**
   * The blend factor related to the water color (reflection, far)
   */
  float colorBlendFactor2;
  /**
   * Represents the maximum length of a wave
   */
  float waveLength;
  /**
   * Defines the waves speed
   */
  float waveSpeed;
  /**
   * Sets or gets whether or not automatic clipping should be enabled or not. Setting to true will
   * save performances and will avoid calculating useless pixels in the pixel shader of the water
   * material.
   */
  bool disableClipPlane;

  ReadOnlyProperty<WaterMaterial, RenderTargetTexturePtr> refractionTexture;
  ReadOnlyProperty<WaterMaterial, RenderTargetTexturePtr> reflectionTexture;
  ReadOnlyProperty<WaterMaterial, bool> renderTargetsEnabled;
  ReadOnlyProperty<WaterMaterial, float> lastTime;

protected:
  std::vector<RenderTargetTexturePtr> _renderTargets;

private:
  BaseTexturePtr _bumpTexture;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;

  /**
   * @param Add a smaller moving bump to less steady waves.
   */
  bool _bumpSuperimpose;

  /**
   * @param Color refraction and reflection differently with .waterColor2 and
   * .colorBlendFactor2. Non-linear (physically correct)
   * fresnel.
   */
  bool _fresnelSeparate;

  /**
   * @param Bump Waves modify the reflection.
   */
  bool _bumpAffectsReflection;

  AbstractMesh* _mesh;
  RenderTargetTexturePtr _refractionRTT;
  RenderTargetTexturePtr _reflectionRTT;
  Matrix _reflectionTransform;
  float _lastTime;
  float _lastDeltaTime;
  // Needed for callbacks
  bool _isVisible;
  std::optional<Plane> _clipPlane;
  Matrix _savedViewMatrix;
  Matrix _mirrorMatrix;
  bool _useLogarithmicDepth;
  ImageProcessingConfigurationPtr _imageProcessingConfiguration;
  Observer<ImageProcessingConfiguration>::Ptr _imageProcessingObserver;

}; // end of class WaterMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_H
