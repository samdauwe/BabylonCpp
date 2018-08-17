#ifndef BABYLON_MATERIALS_MATERIAL_H
#define BABYLON_MATERIALS_MATERIAL_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief Base class for the main features of a material in Babylon.js.
 */
class BABYLON_SHARED_EXPORT Material : public IAnimatable {

private:
  // Triangle views
  static constexpr unsigned int _TriangleFillMode  = 0;
  static constexpr unsigned int _WireFrameFillMode = 1;
  static constexpr unsigned int _PointFillMode     = 2;
  // Draw modes
  static constexpr unsigned int _PointListDrawMode     = 3;
  static constexpr unsigned int _LineListDrawMode      = 4;
  static constexpr unsigned int _LineLoopDrawMode      = 5;
  static constexpr unsigned int _LineStripDrawMode     = 6;
  static constexpr unsigned int _TriangleStripDrawMode = 7;
  static constexpr unsigned int _TriangleFanDrawMode   = 8;

  /**
   * Stores the clock-wise side orientation
   */
  static constexpr unsigned int _ClockWiseSideOrientation = 0;

  /**
   * Stores the counter clock-wise side orientation
   */
  static constexpr unsigned int _CounterClockWiseSideOrientation = 1;

  /**
   * The dirty texture flag value
   */
  static constexpr unsigned int _TextureDirtyFlag = 1;

  /**
   * The dirty light flag value
   */
  static constexpr unsigned int _LightDirtyFlag = 2;

  /**
   * The dirty fresnel flag value
   */
  static constexpr unsigned int _FresnelDirtyFlag = 4;

  /**
   * The dirty attribute flag value
   */
  static constexpr unsigned int _AttributesDirtyFlag = 8;

  /**
   * The dirty misc flag value
   */
  static constexpr unsigned int _MiscDirtyFlag = 16;

public:
  /**
   * @brief Returns the triangle fill mode.
   */
  static constexpr unsigned int TriangleFillMode()
  {
    return Material::_TriangleFillMode;
  }

  /**
   * @brief Returns the wireframe mode.
   */
  static constexpr unsigned int WireFrameFillMode()
  {
    return Material::_WireFrameFillMode;
  }

  /**
   * @brief Returns the point fill mode.
   */
  static constexpr unsigned int PointFillMode()
  {
    return Material::_PointFillMode;
  }

  /**
   * @brief Returns the point list draw mode.
   */
  static constexpr unsigned int PointListDrawMode()
  {
    return Material::_PointListDrawMode;
  }

  /**
   * @brief Returns the line list draw mode.
   */
  static constexpr unsigned int LineListDrawMode()
  {
    return Material::_LineListDrawMode;
  }

  /**
   * @brief Returns the line loop draw mode.
   */
  static constexpr unsigned int LineLoopDrawMode()
  {
    return Material::_LineLoopDrawMode;
  }

  /**
   * @brief Returns the line strip draw mode.
   */
  static constexpr unsigned int LineStripDrawMode()
  {
    return Material::_LineStripDrawMode;
  }

  /**
   * @brief Returns the triangle strip draw mode.
   */
  static constexpr unsigned int TriangleStripDrawMode()
  {
    return Material::_TriangleStripDrawMode;
  }

  /**
   * @brief Returns the triangle fan draw mode.
   */
  static constexpr unsigned int TriangleFanDrawMode()
  {
    return Material::_TriangleFanDrawMode;
  }

  /**
   * @brief Returns the clock-wise side orientation.
   */
  static constexpr unsigned int ClockWiseSideOrientation()
  {
    return Material::_ClockWiseSideOrientation;
  }

  /**
   * @brief Returns the counter clock-wise side orientation.
   */
  static constexpr unsigned int CounterClockWiseSideOrientation()
  {
    return Material::_CounterClockWiseSideOrientation;
  }

  /**
   * @brief Returns the dirty texture flag value.
   */
  static constexpr unsigned int TextureDirtyFlag()
  {
    return Material::_TextureDirtyFlag;
  }

  /**
   * @brief Returns the dirty light flag value.
   */
  static constexpr unsigned int LightDirtyFlag()
  {
    return Material::_LightDirtyFlag;
  }

  /**
   * @brief Returns the dirty fresnel flag value.
   */
  static constexpr unsigned int FresnelDirtyFlag()
  {
    return Material::_FresnelDirtyFlag;
  }

  /**
   * @brief Returns the dirty attributes flag value.
   */
  static constexpr unsigned int AttributesDirtyFlag()
  {
    return Material::_AttributesDirtyFlag;
  }

  /**
   * @brief Returns the dirty misc flag value.
   */
  static constexpr unsigned int MiscDirtyFlag()
  {
    return Material::_MiscDirtyFlag;
  }

  virtual ~Material() override;

  virtual IReflect::Type type() const override;

  /**
   * @param Returns a string representation of the current material.
   * @param fullDetails defines a boolean indicating which levels of logging is
   * desired
   * @returns a string with material information
   */
  string_t toString(bool fullDetails = false) const;

  /**
   * @param Gets the class name of the material.
   * @returns a string with the class name of the material
   */
  virtual const string_t getClassName() const;

  /**
   * @brief Specifies if updates for the material been locked.
   */
  bool isFrozen() const;

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
  Effect* getEffect();

  /**
   * @brief Returns the current scene.
   * @returns a Scene
   */
  Scene* getScene() const;

  /**
   * @brief Specifies if the material will require alpha blending.
   * @returns a boolean specifying if alpha blending is needed
   */
  virtual bool needAlphaBlending() const;

  /**
   * @brief Specifies if the mesh will require alpha blending.
   * @param mesh defines the mesh to check
   * @returns a boolean specifying if alpha blending is needed for the mesh
   */
  virtual bool needAlphaBlendingForMesh(AbstractMesh* mesh) const;

  /**
   * @brief Specifies if this material should be rendered in alpha test mode.
   * @returns a boolean specifying if an alpha test is needed.
   */
  virtual bool needAlphaTesting() const;

  /**
   * @brief Gets the texture used for the alpha test.
   * @returns the texture to use for alpha testing
   */
  virtual BaseTexture* getAlphaTestTexture();

  /**
   * @brief Marks the material to indicate that it needs to be re-calculated.
   */
  void markDirty();

  /**
   * @brief Hidden
   */
  bool _preBind(Effect* effect                             = nullptr,
                Nullable<unsigned int> overrideOrientation = nullptr);

  /**
   * @brief Binds the material to the mesh.
   * @param world defines the world transformation matrix
   * @param mesh defines the mesh to bind the material to
   */
  virtual void bind(Matrix* world, Mesh* mesh);

  /**
   * @brief Binds the submesh to the material.
   * @param world defines the world transformation matrix
   * @param mesh defines the mesh containing the submesh
   * @param subMesh defines the submesh to bind the material to
   */
  virtual void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh);

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
  void bindViewProjection(Effect* effect);

  /**
   * @brief Unbinds the material from the mesh.
   */
  virtual void unbind();

  /**
   * @brief Gets the active textures from the material.
   * @returns an array of textures
   */
  virtual vector_t<BaseTexture*> getActiveTextures() const;

  /**
   * @brief Specifies if the material uses a texture.
   * @param texture defines the texture to check against the material
   * @returns a boolean specifying if the material uses the texture
   */
  virtual bool hasTexture(BaseTexture* texture) const;

  /**
   * @brief akes a duplicate of the material, and gives it a new name.
   * @param name defines the new name for the duplicated material
   * @returns the cloned material
   */
  virtual Material* clone(const string_t& name,
                          bool cloneChildren = false) const;

  /**
   * @brief Gets the meshes bound to the material.
   * @returns an array of meshes bound to the material
   */
  vector_t<AbstractMesh*> getBindedMeshes();

  /**
   * @brief Force shader compilation
   * @param mesh defines the mesh associated with this material
   * @param onCompiled defines a function to execute once the material is
   * compiled
   * @param options defines the options to configure the compilation
   */
  void
  forceCompilation(AbstractMesh* mesh,
                   const ::std::function<void(Material* material)>& onCompiled,
                   Nullable<bool> clipPlane = false);

  /**
   * @brief Marks a define in the material to indicate that it needs to be
   * re-computed.
   * @param flag defines a flag used to determine which parts of the material
   * have to be marked as dirty
   */
  virtual void markAsDirty(unsigned int flag) override;

  /**
   * @brief Disposes the material
   * @param forceDisposeEffect specifies if effects should be forcefully
   * disposed
   * @param forceDisposeTextures specifies if textures should be forcefully
   * disposed
   */
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false);

  /**
   * Serializes this material.
   * @returns the serialized material object
   */
  Json::object serialize() const;

  virtual void trackCreation(
    const ::std::function<void(const Effect* effect)>& onCompiled,
    const ::std::function<void(const Effect* effect, const string_t& errors)>&
      onError);

  void addMaterialToScene(unique_ptr_t<Material>&& newMaterial);
  void addMultiMaterialToScene(unique_ptr_t<MultiMaterial>&& newMultiMaterial);
  virtual vector_t<Animation*> getAnimations() override;
  void copyTo(Material* other) const;

  /**
   * @brief Creates a MultiMaterial from parsed MultiMaterial data.
   * @param parsedMultiMaterial defines parsed MultiMaterial data.
   * @param scene defines the hosting scene
   * @returns a new MultiMaterial
   */
  static MultiMaterial*
  ParseMultiMaterial(const Json::value& parsedMultiMaterial, Scene* scene);

  /**
   * @brief Creates a material from parsed material data.
   * @param parsedMaterial defines parsed material data
   * @param scene defines the hosting scene
   * @param rootUrl defines the root URL to use to load textures
   * @returns a new material
   */
  static Material* Parse(const Json::value& parsedMaterial, Scene* scene,
                         const string_t& rootUrl);

protected:
  /**
   * @brief Creates a material instance.
   * @param name defines the name of the material
   * @param scene defines the scene to reference
   * @param doNotAdd specifies if the material should be added to the scene
   */
  Material(const string_t& name, Scene* scene, bool doNotAdd = false);

protected:
  /**
   * @brief Sets the alpha value of the material.
   */
  void set_alpha(float value);

  /**
   * @brief Gets the alpha value of the material.
   */
  float get_alpha() const;

  /**
   * @brief Sets the back-face culling state.
   */
  void set_backFaceCulling(bool value);

  /**
   * @brief Gets the back-face culling state.
   */
  bool get_backFaceCulling() const;

  /**
   * @brief Called during a dispose event.
   */
  void
  set_onDispose(const ::std::function<void(Material*, EventState&)>& callback);

  /**
   * @brief An event triggered when the material is bound.
   */
  Observable<AbstractMesh>& get_onBindObservable();

  /**
   * @brief Called during a bind event.
   */
  void
  set_onBind(const ::std::function<void(AbstractMesh*, EventState&)>& callback);

  /**
   * @brief An event triggered when the material is unbound.
   */
  Observable<Material>& get_onUnBindObservable();

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
  unsigned int get_alphaMode() const;

  /**
   * @brief Sets the need depth pre-pass value.
   */
  void set_needDepthPrePass(bool value);

  /**
   * @brief Gets the depth pre-pass value.
   */
  bool get_needDepthPrePass() const;

  /**
   * @brief Sets the state for enabling fog.
   */
  void set_fogEnabled(bool value);

  /**
   * @brief Gets the value of the fog enabled state.
   */
  bool get_fogEnabled() const;

  /**
   * @brief Sets the state of wireframe mode.
   */
  void set_wireframe(bool value);

  /**
   * @brief Gets a value specifying if wireframe mode is enabled.
   */
  bool get_wireframe() const;

  /**
   * @brief Sets the state of point cloud mode.
   */
  void set_pointsCloud(bool value);

  /**
   * @brief Gets the value specifying if point clouds are enabled.
   */
  bool get_pointsCloud() const;

  /**
   * @brief Sets the material fill mode.
   */
  void set_fillMode(unsigned int value);

  /**
   * @brief Gets the material fill mode.
   */
  unsigned int get_fillMode() const;

  /**
   * @brief Gets the logarithmic depth setting.
   */
  virtual bool get_useLogarithmicDepth() const;

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
  void _markAllSubMeshesAsDirty(
    const ::std::function<void(MaterialDefines& defines)>& func);

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

public:
  // Events
  /**
   * An event triggered when the material is disposed
   */
  Observable<Material> onDisposeObservable;

  /**
   * The ID of the material
   */
  string_t id;

  /**
   * Gets or sets the unique id of the material
   */
  size_t uniqueId;

  /**
   * The name of the material
   */
  string_t name;

  /**
   * Specifies if the ready state should be checked on each call
   */
  bool checkReadyOnEveryCall;

  /**
   * Specifies if the ready state should be checked once
   */
  bool checkReadyOnlyOnce;

  /**
   * The alpha value of the material
   */
  Property<Material, float> alpha;

  /**
   * The back-face culling state
   */
  Property<Material, bool> backFaceCulling;

  /**
   * Stores the value for side orientation
   */
  int sideOrientation;

  /**
   * Callback triggered when the material is compiled
   */
  ::std::function<void(const Effect* effect)> onCompiled;

  /**
   * Callback triggered when an error occurs
   */
  ::std::function<void(const Effect* effect, const string_t& errors)> onError;

  /**
   * Callback triggered to get the render target textures
   */
  ::std::function<vector_t<RenderTargetTexture*>()> getRenderTargetTextures;

  /**
   * Specifies if the material should be serialized
   */
  bool doNotSerialize;

  /**
   * Specifies if the effect should be stored on sub meshes
   */
  bool storeEffectOnSubMeshes;

  /**
   * Stores the animations for the material
   */
  vector_t<Animation*> animations;

  /**
   * Called during a dispose event
   */
  WriteOnlyProperty<Material, ::std::function<void(Material*, EventState&)>>
    onDispose;

  /**
   * An event triggered when the material is bound
   */
  ReadOnlyProperty<Material, Observable<AbstractMesh>> onBindObservable;

  /**
   * Called during a bind event
   */
  WriteOnlyProperty<Material, ::std::function<void(AbstractMesh*, EventState&)>>
    onBind;

  /**
   * An event triggered when the material is unbound
   */
  ReadOnlyProperty<Material, Observable<Material>> onUnBindObservable;

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
   */
  Effect* _effect;

  /**
   * Specifies if the material was previously ready
   */
  bool _wasPreviouslyReady;

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
   * Stores the uniform buffer
   */
  unique_ptr_t<UniformBuffer> _uniformBuffer;

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
  ::std::function<void()> _beforeRenderCallback;

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

}; // end of class Material

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_H
