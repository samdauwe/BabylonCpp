#ifndef BABYLON_MATERIALS_MATERIAL_H
#define BABYLON_MATERIALS_MATERIAL_H

#include <map>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/misc/iinspectable.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class BaseSubMesh;
class BaseTexture;
class Effect;
class Material;
struct MaterialDefines;
class Mesh;
class MultiMaterial;
class RenderTargetTexture;
class Scene;
class SubMesh;
class UniformBuffer;
using AbstractMeshPtr        = std::shared_ptr<AbstractMesh>;
using BaseTexturePtr         = std::shared_ptr<BaseTexture>;
using EffectPtr              = std::shared_ptr<Effect>;
using MaterialPtr            = std::shared_ptr<Material>;
using MultiMaterialPtr       = std::shared_ptr<MultiMaterial>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

/**
 * @brief Options for compiling materials.
 */
struct BABYLON_SHARED_EXPORT IMaterialCompilationOptions {
  /**
   * Defines whether clip planes are enabled.
   */
  bool clipPlane = false;

  /**
   * Defines whether instances are enabled.
   */
  bool useInstances = false;
}; // end of struct IMaterialCompilationOptions

struct BABYLON_SHARED_EXPORT OnCreatedEffectParameters {
  Effect* effect       = nullptr;
  BaseSubMesh* subMesh = nullptr;
}; // end of struct OnCreatedEffectParameters

/**
 * @brief Base class for the main features of a material in Babylon.js.
 */
class BABYLON_SHARED_EXPORT Material : public IAnimatable {

private:
  using MaterialDefinesCallback = std::function<void(MaterialDefines& defines)>;

public:
  /**
   * @brief Returns the triangle fill mode.
   */
  static constexpr unsigned int TriangleFillMode = Constants::MATERIAL_TriangleFillMode;

  /**
   * @brief Returns the wireframe mode.
   */
  static constexpr unsigned int WireFrameFillMode = Constants::MATERIAL_WireFrameFillMode;

  /**
   * @brief Returns the point fill mode.
   */
  static constexpr unsigned int PointFillMode = Constants::MATERIAL_PointFillMode;

  /**
   * @brief Returns the point list draw mode.
   */
  static constexpr unsigned int PointListDrawMode = Constants::MATERIAL_PointListDrawMode;

  /**
   * @brief Returns the line list draw mode.
   */
  static constexpr unsigned int LineListDrawMode = Constants::MATERIAL_LineListDrawMode;

  /**
   * @brief Returns the line loop draw mode.
   */
  static constexpr unsigned int LineLoopDrawMode = Constants::MATERIAL_LineLoopDrawMode;

  /**
   * @brief Returns the line strip draw mode.
   */
  static constexpr unsigned int LineStripDrawMode = Constants::MATERIAL_LineStripDrawMode;

  /**
   * @brief Returns the triangle strip draw mode.
   */
  static constexpr unsigned int TriangleStripDrawMode = Constants::MATERIAL_TriangleStripDrawMode;

  /**
   * @brief Returns the triangle fan draw mode.
   */
  static constexpr unsigned int TriangleFanDrawMode = Constants::MATERIAL_TriangleFanDrawMode;

  /**
   * @brief Returns the clock-wise side orientation.
   */
  static constexpr unsigned int ClockWiseSideOrientation
    = Constants::MATERIAL_ClockWiseSideOrientation;

  /**
   * @brief Returns the counter clock-wise side orientation.
   */
  static constexpr unsigned int CounterClockWiseSideOrientation
    = Constants::MATERIAL_CounterClockWiseSideOrientation;

  /**
   * The dirty texture flag value
   */
  static constexpr unsigned int TextureDirtyFlag = Constants::MATERIAL_TextureDirtyFlag;

  /**
   * The dirty light flag value
   */
  static constexpr unsigned int LightDirtyFlag = Constants::MATERIAL_LightDirtyFlag;

  /**
   * The dirty fresnel flag value
   */
  static constexpr unsigned int FresnelDirtyFlag = Constants::MATERIAL_FresnelDirtyFlag;

  /**
   * The dirty attribute flag value
   */
  static constexpr unsigned int AttributesDirtyFlag = Constants::MATERIAL_AttributesDirtyFlag;

  /**
   * The dirty misc flag value
   */
  static constexpr unsigned int MiscDirtyFlag = Constants::MATERIAL_MiscDirtyFlag;

  /**
   * The all dirty flag value
   */
  static constexpr unsigned int AllDirtyFlag = Constants::MATERIAL_AllDirtyFlag;

  /**
   * MaterialTransparencyMode: No transparency mode, Alpha channel is not use.
   */
  static constexpr unsigned int MATERIAL_OPAQUE = 0;

  /**
   * MaterialTransparencyMode: Alpha Test mode, pixel are discarded below a certain threshold
   * defined by the alpha cutoff value.
   */
  static constexpr unsigned int MATERIAL_ALPHATEST = 1;

  /**
   * MaterialTransparencyMode: Pixels are blended (according to the alpha mode) with the already
   * drawn pixels in the current frame buffer.
   */
  static constexpr unsigned int MATERIAL_ALPHABLEND = 2;

  /**
   * MaterialTransparencyMode: Pixels are blended (according to the alpha mode) with the already
   * drawn pixels in the current frame buffer. They are also discarded below the alpha cutoff
   * threshold to improve performances.
   */
  static constexpr unsigned int MATERIAL_ALPHATESTANDBLEND = 3;

  ~Material() override; // = default

  [[nodiscard]] Type type() const override;

  /**
   * @brief Gets a property.
   */
  AnimationValue getProperty(const std::vector<std::string>& targetPropertyPath) override;

  /**
   * @brief Sets a property.
   */
  void setProperty(const std::vector<std::string>& targetPropertyPath,
                   const AnimationValue& value) override;

  /**
   * @param Returns a string representation of the current material.
   * @param fullDetails defines a boolean indicating which levels of logging is
   * desired
   * @returns a string with material information
   */
  [[nodiscard]] std::string toString(bool fullDetails = false) const;

  /**
   * @param Gets the class name of the material.
   * @returns a string with the class name of the material
   */
  [[nodiscard]] virtual std::string getClassName() const;

  /**
   * @brief Specifies if updates for the material been locked.
   */
  [[nodiscard]] bool isFrozen() const;

  /**
   * @brief Locks updates for the material.
   */
  void freeze();

  /**
   * @brief Unlocks updates for the material.
   */
  void unfreeze();

  /**
   * @brief Specifies if the material is ready to be used.
   * @param mesh defines the mesh to check
   * @param useInstances specifies if instances should be used
   * @returns a boolean indicating if the material is ready to be used
   */
  virtual bool isReady(AbstractMesh* mesh = nullptr, bool useInstances = false);

  /**
   * @brief Specifies that the submesh is ready to be used.
   * @param mesh defines the mesh to check
   * @param subMesh defines which submesh to check
   * @param useInstances specifies that instances should be used
   * @returns a boolean indicating that the submesh is ready or not
   */
  virtual bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                 bool useInstances = false);

  /**
   * @brief Returns the material effect.
   * @returns the effect associated with the material
   */
  EffectPtr& getEffect();

  /**
   * @brief Returns the current scene.
   * @returns a Scene
   */
  [[nodiscard]] Scene* getScene() const;

  /**
   * @brief Gets the current transparency mode.
   */
  std::optional<unsigned int>& get_transparencyMode();

  /**
   * @brief Sets the transparency mode of the material.
   *
   * | Value | Type                                | Description |
   * | ----- | ----------------------------------- | ----------- |
   * | 0     | OPAQUE                              |             |
   * | 1     | ALPHATEST                           |             |
   * | 2     | ALPHABLEND                          |             |
   * | 3     | ALPHATESTANDBLEND                   |             |
   *
   */
  void set_transparencyMode(const std::optional<unsigned int>& value);

  /**
   * @brief Specifies if the material will require alpha blending.
   * @returns a boolean specifying if alpha blending is needed
   */
  [[nodiscard]] virtual bool needAlphaBlending() const;

  /**
   * @brief Specifies if the mesh will require alpha blending.
   * @param mesh defines the mesh to check
   * @returns a boolean specifying if alpha blending is needed for the mesh
   */
  [[nodiscard]] virtual bool needAlphaBlendingForMesh(const AbstractMesh& mesh) const;

  /**
   * @brief Specifies if this material should be rendered in alpha test mode.
   * @returns a boolean specifying if an alpha test is needed.
   */
  [[nodiscard]] virtual bool needAlphaTesting() const;

  /**
   * @brief Gets the texture used for the alpha test.
   * @returns the texture to use for alpha testing
   */
  virtual BaseTexturePtr getAlphaTestTexture();

  /**
   * @brief Marks the material to indicate that it needs to be re-calculated.
   */
  void markDirty();

  /**
   * @brief Hidden
   */
  bool _preBind(const EffectPtr& effect                         = nullptr,
                std::optional<unsigned int> overrideOrientation = std::nullopt);

  /**
   * @brief Binds the material to the mesh.
   * @param world defines the world transformation matrix
   * @param mesh defines the mesh to bind the material to
   */
  virtual void bind(Matrix& world, Mesh* mesh);

  /**
   * @brief Binds the submesh to the material.
   * @param world defines the world transformation matrix
   * @param mesh defines the mesh containing the submesh
   * @param subMesh defines the submesh to bind the material to
   */
  virtual void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh);

  /**
   * @brief Binds the world matrix to the material.
   * @param world defines the world transformation matrix
   */
  virtual void bindOnlyWorldMatrix(Matrix& world);

  /**
   * @brief Binds the scene's uniform buffer to the effect.
   * @param effect defines the effect to bind to the scene uniform buffer
   * @param sceneUbo defines the uniform buffer storing scene data
   */
  void bindSceneUniformBuffer(Effect* effect, UniformBuffer* sceneUbo);

  /**
   * @brief Binds the view matrix to the effect.
   * @param effect defines the effect to bind the view matrix to
   */
  void bindView(Effect* effect);

  /**
   * @brief Binds the view projection matrix to the effect.
   * @param effect defines the effect to bind the view projection matrix to
   */
  void bindViewProjection(const EffectPtr& effect);

  /**
   * @brief Unbinds the material from the mesh.
   */
  virtual void unbind();

  /**
   * @brief Gets the active textures from the material.
   * @returns an array of textures
   */
  [[nodiscard]] virtual std::vector<BaseTexturePtr> getActiveTextures() const;

  /**
   * @brief Specifies if the material uses a texture.
   * @param texture defines the texture to check against the material
   * @returns a boolean specifying if the material uses the texture
   */
  [[nodiscard]] virtual bool hasTexture(const BaseTexturePtr& texture) const;

  /**
   * @brief akes a duplicate of the material, and gives it a new name.
   * @param name defines the new name for the duplicated material
   * @returns the cloned material
   */
  [[nodiscard]] virtual MaterialPtr clone(const std::string& name,
                                          bool cloneChildren = false) const;

  /**
   * @brief Gets the meshes bound to the material.
   * @returns an array of meshes bound to the material
   */
  std::vector<AbstractMeshPtr> getBindedMeshes();

  /**
   * @brief Force shader compilation.
   * @param mesh defines the mesh associated with this material
   * @param onCompiled defines a function to execute once the material is compiled
   * @param options defines the options to configure the compilation
   * @param onError defines a function to execute if the material fails compiling
   */
  void forceCompilation(AbstractMesh* mesh,
                        const std::function<void(Material* material)>& onCompiled,
                        const std::optional<IMaterialCompilationOptions>& options = std::nullopt,
                        const std::function<void(const std::string& reason)>& onError = nullptr);

  /**
   * @brief Marks a define in the material to indicate that it needs to be
   * re-computed.
   * @param flag defines a flag used to determine which parts of the material
   * have to be marked as dirty
   */
  void markAsDirty(unsigned int flag) override;

  /**
   * @brief Disposes the material.
   * @param forceDisposeEffect specifies if effects should be forcefully
   * disposed
   * @param forceDisposeTextures specifies if textures should be forcefully
   * disposed
   * @param notBoundToMesh specifies if the material that is being disposed is
   * known to be not bound to any mesh
   */
  virtual void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
                       bool notBoundToMesh = false);

  /**
   * Serializes this material.
   * @returns the serialized material object
   */
  [[nodiscard]] json serialize() const;

  virtual void trackCreation(
    const std::function<void(const EffectPtr& effect)>& onCompiled,
    const std::function<void(const EffectPtr& effect, const std::string& errors)>& onError);

  void addMaterialToScene(const MaterialPtr& newMaterial);
  void addMultiMaterialToScene(const MultiMaterialPtr& newMultiMaterial);
  std::vector<AnimationPtr> getAnimations() override;
  void copyTo(Material* other) const;

  /**
   * @brief Creates a MultiMaterial from parsed MultiMaterial data.
   * @param parsedMultiMaterial defines parsed MultiMaterial data.
   * @param scene defines the hosting scene
   * @returns a new MultiMaterial
   */
  static MultiMaterialPtr ParseMultiMaterial(const json& parsedMultiMaterial, Scene* scene);

  /**
   * @brief Creates a material from parsed material data.
   * @param parsedMaterial defines parsed material data
   * @param scene defines the hosting scene
   * @param rootUrl defines the root URL to use to load textures
   * @returns a new material
   */
  static MaterialPtr Parse(const json& parsedMaterial, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Creates a material instance.
   * @param name defines the name of the material
   * @param scene defines the scene to reference
   * @param doNotAdd specifies if the material should be added to the scene
   */
  Material(const std::string& name, Scene* scene, bool doNotAdd = false);

  /**
   * @brief Returns true if alpha blending should be disabled.
   */
  bool get__disableAlphaBlending() const;

protected:
  /**
   * @brief Sets the alpha value of the material.
   */
  void set_alpha(float value);

  /**
   * @brief Gets the alpha value of the material.
   */
  [[nodiscard]] float get_alpha() const;

  /**
   * @brief Sets the back-face culling state.
   */
  void set_backFaceCulling(bool value);

  /**
   * @brief Gets the back-face culling state.
   */
  [[nodiscard]] bool get_backFaceCulling() const;

  /**
   * @brief Gets a boolean indicating that current material needs to register
   * RTT.
   */
  [[nodiscard]] virtual bool get_hasRenderTargetTextures() const;

  /**
   * @brief Called during a dispose event.
   */
  void set_onDispose(const std::function<void(Material*, EventState&)>& callback);

  /**
   * @brief An event triggered when the material is bound.
   */
  Observable<AbstractMesh>& get_onBindObservable();

  /**
   * @brief Called during a bind event.
   */
  void set_onBind(const std::function<void(AbstractMesh*, EventState&)>& callback);

  /**
   * @brief An event triggered when the material is unbound.
   */
  Observable<Material>& get_onUnBindObservable();

  /**
   * @brief An event triggered when the effect is (re)created.
   */
  Observable<OnCreatedEffectParameters>& get_onEffectCreatedObservable();

  /**
   * @brief Sets the value of the alpha mode.
   *
   * | Value | Type | Description |
   * | --- | --- | --- |
   * | 0 | ALPHA_DISABLE |   |
   * | 1 | ALPHA_ADD |   |
   * | 2 | ALPHA_COMBINE |   |
   * | 3 | ALPHA_SUBTRACT |   |
   * | 4 | ALPHA_MULTIPLY |   |
   * | 5 | ALPHA_MAXIMIZED |   |
   * | 6 | ALPHA_ONEONE |   |
   * | 7 | ALPHA_PREMULTIPLIED |   |
   * | 8 | ALPHA_PREMULTIPLIED_PORTERDUFF |   |
   * | 9 | ALPHA_INTERPOLATE |   |
   * | 10 | ALPHA_SCREENMODE |   |
   *
   */
  void set_alphaMode(unsigned int value);

  /**
   * @brief Gets the value of the alpha mode.
   */
  [[nodiscard]] unsigned int get_alphaMode() const;

  /**
   * @brief Sets the need depth pre-pass value.
   */
  void set_needDepthPrePass(bool value);

  /**
   * @brief Gets the depth pre-pass value.
   */
  [[nodiscard]] bool get_needDepthPrePass() const;

  /**
   * @brief Sets the state for enabling fog.
   */
  void set_fogEnabled(bool value);

  /**
   * @brief Gets the value of the fog enabled state.
   */
  [[nodiscard]] bool get_fogEnabled() const;

  /**
   * @brief Sets the state of wireframe mode.
   */
  void set_wireframe(bool value);

  /**
   * @brief Gets a value specifying if wireframe mode is enabled.
   */
  [[nodiscard]] bool get_wireframe() const;

  /**
   * @brief Sets the state of point cloud mode.
   */
  void set_pointsCloud(bool value);

  /**
   * @brief Gets the value specifying if point clouds are enabled.
   */
  [[nodiscard]] bool get_pointsCloud() const;

  /**
   * @brief Sets the material fill mode.
   */
  void set_fillMode(unsigned int value);

  /**
   * @brief Gets the material fill mode.
   */
  [[nodiscard]] unsigned int get_fillMode() const;

  /**
   * @brief Gets the logarithmic depth setting.
   */
  [[nodiscard]] virtual bool get_useLogarithmicDepth() const;

  /**
   * @brief Sets the logarithmic depth setting.
   */
  virtual void set_useLogarithmicDepth(bool value);

protected:
  /**
   * @brief Specifies if material alpha testing should be turned on for the
   * mesh.
   * @param mesh defines the mesh to check
   */
  bool _shouldTurnAlphaTestOn(AbstractMesh* mesh) const;

  /**
   * @brief Processes to execute after binding the material to a mesh.
   * @param mesh defines the rendered mesh
   */
  void _afterBind(Mesh* mesh);

  /**
   * @brief Marks all submeshes of a material to indicate that their material
   * defines need to be re-calculated.
   * @param func defines a function which checks material defines against the
   * submeshes
   */
  void _markAllSubMeshesAsDirty(const MaterialDefinesCallback& func);

  /**
   * @brief Indicates that we need to re-calculated for all submeshes.
   */
  void _markAllSubMeshesAsAllDirty();

  /**
   * @brief Indicates that image processing needs to be re-calculated for all
   * submeshes.
   */
  void _markAllSubMeshesAsImageProcessingDirty();

  /**
   * @brief Indicates that textures need to be re-calculated for all submeshes.
   */
  void _markAllSubMeshesAsTexturesDirty();

  /**
   * @brief Indicates that fresnel needs to be re-calculated for all submeshes.
   */
  void _markAllSubMeshesAsFresnelDirty();

  /**
   * @brief Indicates that fresnel and misc need to be re-calculated for all
   * submeshes.
   */
  void _markAllSubMeshesAsFresnelAndMiscDirty();

  /**
   * @brief Indicates that lights need to be re-calculated for all submeshes.
   */
  void _markAllSubMeshesAsLightsDirty();

  /**
   * @brief Indicates that attributes need to be re-calculated for all
   * submeshes.
   */
  void _markAllSubMeshesAsAttributesDirty();

  /**
   * @brief Indicates that misc needs to be re-calculated for all submeshes.
   */
  void _markAllSubMeshesAsMiscDirty();

  /**
   * @brief Indicates that textures and misc need to be re-calculated for all
   * submeshes.
   */
  void _markAllSubMeshesAsTexturesAndMiscDirty();

private:
  void releaseVertexArrayObject(const AbstractMeshPtr& mesh, bool forceDisposeEffect = false);

public:
  // Events
  /**
   * An event triggered when the material is disposed
   */
  Observable<Material> onDisposeObservable;

  /**
   * Custom callback helping to override the default shader used in the
   * material.
   */
  std::function<std::string(const std::string& shaderName, std::vector<std::string>& uniforms,
                            std::vector<std::string>& uniformBuffers,
                            std::vector<std::string>& samplers, MaterialDefines& defines,
                            std::vector<std::string>& attributes)>
    customShaderNameResolve;

  /**
   * The ID of the material
   */
  std::string id;

  /**
   * Gets or sets the unique id of the material
   */
  size_t uniqueId;

  /**
   * The name of the material
   */
  std::string name;

  /**
   * Specifies if the ready state should be checked on each call
   */
  bool checkReadyOnEveryCall;

  /**
   * Specifies if the ready state should be checked once
   */
  bool checkReadyOnlyOnce;

  /**
   * List of inspectable custom properties (used by the Inspector)
   * @see https://doc.babylonjs.com/how_to/debug_layer#extensibility
   */
  std::vector<IInspectable> inspectableCustomProperties;

  /**
   * The alpha value of the material
   */
  Property<Material, float> alpha;

  /**
   * The back-face culling state
   */
  Property<Material, bool> backFaceCulling;

  /**
   * Gets a boolean indicating that current material needs to register RTT
   */
  ReadOnlyProperty<Material, bool> hasRenderTargetTextures;

  /**
   * Stores the value for side orientation
   */
  int sideOrientation;

  /**
   * Callback triggered when the material is compiled
   */
  std::function<void(const Effect* effect)> onCompiled;

  /**
   * Callback triggered when an error occurs
   */
  std::function<void(const Effect* effect, const std::string& errors)> onError;

  /**
   * Callback triggered to get the render target textures
   */
  std::function<std::vector<RenderTargetTexturePtr>()> getRenderTargetTextures;

  /**
   * Specifies if the material should be serialized
   */
  bool doNotSerialize;

  /**
   * Specifies if the effect should be stored on sub meshes
   */
  bool _storeEffectOnSubMeshes;

  /**
   * Stores the animations for the material
   */
  std::vector<AnimationPtr> animations;

  /**
   * Called during a dispose event
   */
  WriteOnlyProperty<Material, std::function<void(Material*, EventState&)>> onDispose;

  /**
   * An event triggered when the material is bound
   */
  ReadOnlyProperty<Material, Observable<AbstractMesh>> onBindObservable;

  /**
   * Called during a bind event
   */
  WriteOnlyProperty<Material, std::function<void(AbstractMesh*, EventState&)>> onBind;

  /**
   * An event triggered when the material is unbound
   */
  ReadOnlyProperty<Material, Observable<Material>> onUnBindObservable;

  /**
   * An event triggered when the effect is (re)created
   */
  ReadOnlyProperty<Material, Observable<OnCreatedEffectParameters>> onEffectCreatedObservable;

  /**
   * The value of the alpha mode
   */
  Property<Material, unsigned int> alphaMode;

  /**
   * The need depth pre-pass value
   */
  Property<Material, bool> needDepthPrePass;

  /**
   * Specifies if depth writing should be disabled
   */
  bool disableDepthWrite;

  /**
   * Specifies if depth writing should be forced
   */
  bool forceDepthWrite;

  /**
   * Specifies the depth function that should be used. 0 means the default engine function
   */
  int depthFunction;

  /**
   * Specifies if there should be a separate pass for culling
   */
  bool separateCullingPass;

  /**
   * The state for enabling fog
   */
  Property<Material, bool> fogEnabled;

  /**
   * Stores the size of points
   */
  float pointSize;

  /**
   * Stores the z offset value
   */
  float zOffset;

  /**
   * The state of wireframe mode
   */
  Property<Material, bool> wireframe;

  /**
   * The state of point cloud mode
   */
  Property<Material, bool> pointsCloud;

  /**
   * The material fill mode
   */
  Property<Material, unsigned int> fillMode;

  /**
   * Stores the effects for the material
   * Hidden
   */
  EffectPtr _effect;

  /**
   * Hidden
   */
  int _indexInSceneMaterialArray;

  /**
   * Hidden
   */
  std::unordered_map<std::string, AbstractMeshPtr> meshMap;

  /**
   * Gets or sets the current transparency mode.
   */
  Property<Material, std::optional<unsigned int>> transparencyMode;

  /**
   * The logarithmic depth setting.
   */
  Property<Material, bool> useLogarithmicDepth;

protected:
  /**
   * The alpha value of the material
   */
  float _alpha;

  /**
   * Specifies if back face culling is enabled
   */
  bool _backFaceCulling;

  /**
   * An event triggered when the effect is (re)created
   */
  Observable<OnCreatedEffectParameters> _onEffectCreatedObservable;

  /**
   * Stores the uniform buffer
   */
  std::unique_ptr<UniformBuffer> _uniformBuffer;

  /**
   * Enforces alpha test in opaque or blend mode in order to improve the performances of some
   * situations.
   */
  bool _forceAlphaTest;

  /**
   * The transparency mode of the material.
   */
  std::optional<unsigned int> _transparencyMode;

  /**
   * Returns true if alpha blending should be disabled.
   */
  ReadOnlyProperty<Material, bool> _disableAlphaBlending;

private:
  /**
   * An observer which watches for dispose events
   */
  Observer<Material>::Ptr _onDisposeObserver;

  Observable<Material> _onUnBindObservable;
  Observable<AbstractMesh> _onBindObservable;

  /**
   * An observer which watches for bind events
   */
  Observer<AbstractMesh>::Ptr _onBindObserver;
  // Callbacks
  std::function<void()> _beforeRenderCallback;

  /**
   * Stores the value of the alpha mode
   */
  unsigned int _alphaMode;

  /**
   * Stores the state of the need depth pre-pass value
   */
  bool _needDepthPrePass;

  /**
   * Stores the state specifing if fog should be enabled
   */
  bool _fogEnabled;

  /**
   * Specifies if uniform buffers should be used
   */
  bool _useUBO;

  /**
   * Stores a reference to the scene
   */
  Scene* _scene;

  /**
   * Stores the fill mode state
   */
  unsigned int _fillMode;

  /**
   * Specifies if the depth write state should be cached
   */
  bool _cachedDepthWriteState;

  /**
   * Specifies if the depth function state should be cached
   */
  int _cachedDepthFunctionState;

  static const MaterialDefinesCallback _AllDirtyCallBack;
  static const MaterialDefinesCallback _ImageProcessingDirtyCallBack;
  static const MaterialDefinesCallback _TextureDirtyCallBack;
  static const MaterialDefinesCallback _FresnelDirtyCallBack;
  static const MaterialDefinesCallback _MiscDirtyCallBack;
  static const MaterialDefinesCallback _LightsDirtyCallBack;
  static const MaterialDefinesCallback _AttributeDirtyCallBack;
  static const MaterialDefinesCallback _FresnelAndMiscDirtyCallBack;
  static const MaterialDefinesCallback _TextureAndMiscDirtyCallBack;

  static std::vector<MaterialDefinesCallback> _DirtyCallbackArray;
  static const MaterialDefinesCallback _RunDirtyCallBacks;

}; // end of class Material

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_H
