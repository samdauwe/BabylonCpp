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
 * @brief This offers the main features of a material in BJS.
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
   * Stores the clock-wise side orientation.
   */
  static constexpr unsigned int _ClockWiseSideOrientation = 0;

  /**
   * Stores the counter clock-wise side orientation.
   */
  static constexpr unsigned int _CounterClockWiseSideOrientation = 1;

  /**
   * The dirty texture flag value.
   */
  static constexpr unsigned int _TextureDirtyFlag = 1;

  /**
   * The dirty light flag value.
   */
  static constexpr unsigned int _LightDirtyFlag = 2;

  /**
   * The dirty fresnel flag value.
   */
  static constexpr unsigned int _FresnelDirtyFlag = 4;

  /**
   * The dirty attribute flag value.
   */
  static constexpr unsigned int _AttributesDirtyFlag = 8;

  /**
   * The dirty misc flag value.
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
   * @brief Sets the alpha value of the material.
   */
  void setAlpha(float value);

  /**
   * @brief Gets the alpha value of the material.
   */
  float alpha() const;

  /**
   * @brief Sets the back-face culling state.
   */
  void setBackFaceCulling(bool value);

  /**
   * @brief Gets the back-face culling state.
   */
  bool backFaceCulling() const;

  /**
   * @brief Called during a dispose event.
   */
  void
  setOnDispose(const ::std::function<void(Material*, EventState&)>& callback);

  /**
   * @brief Called during a bind event.
   */
  void
  setOnBind(const ::std::function<void(AbstractMesh*, EventState&)>& callback);

  /**
   * @brief Sets the value of the alpha mode.
   */
  void setAlphaMode(unsigned int value);

  /**
   * @brief Gets the value of the alpha mode.
   */
  unsigned int alphaMode() const;

  /**
   * @brief Gets the depth pre-pass value.
   */
  bool needDepthPrePass() const;

  /**
   * @brief Sets the need depth pre-pass value.
   */
  void setNeedDepthPrePass(bool value);

  /**
   * @brief Sets the state for enabling fog.
   */
  void setFogEnabled(bool value);

  /**
   * @brief Gets the value of the fog enabled state.
   */
  bool fogEnabled() const;

  /**
   * @brief Gets a value specifying if wireframe mode is enabled.
   */
  bool wireframe() const;

  /**
   * @brief Sets the state of wireframe mode.
   */
  void setWireframe(bool value);

  /**
   * @brief Gets the value specifying if point clouds are enabled.
   */
  bool pointsCloud() const;

  /**
   * @brief Sets the state of point cloud mode.
   */
  void setPointsCloud(bool value);

  /**
   * @brief Gets the material fill mode.
   */
  unsigned int fillMode() const;

  /**
   * @brief Sets the material fill mode.
   */
  void setFillMode(unsigned int value);

  /**
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading subclasses should override adding information
   * pertainent to themselves.
   * @returns - String with material information.
   */
  string_t toString(bool fullDetails = false) const;

  /**
   * @param Gets the class name of the material.
   * @returns - String with the class name of the material.
   */
  const char* getClassName() const;

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
   * @param mesh - BJS mesh.
   * @param useInstances - Specifies if instances should be used.
   * @returns - Boolean indicating if the material is ready to be used.
   */
  virtual bool isReady(AbstractMesh* mesh = nullptr, bool useInstances = false);

  /**
   * @brief Specifies that the submesh is ready to be used.
   * @param mesh - BJS mesh.
   * @param subMesh - A submesh of the BJS mesh.  Used to check if it is ready.
   * @param useInstances - Specifies that instances should be used.
   * @returns - boolean indicating that the submesh is ready or not.
   */
  virtual bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                 bool useInstances = false);

  /**
   * @brief Returns the material effect.
   * @returns - Nullable material effect.
   */
  Effect* getEffect();

  /**
   * @brief Returns the BJS scene.
   * @returns - BJS Scene.
   */
  Scene* getScene() const;

  /**
   * @brief Specifies if the material will require alpha blending
   * @returns - Boolean specifying if alpha blending is needed.
   */
  virtual bool needAlphaBlending() const;

  /**
   * @brief Specifies if the mesh will require alpha blending.
   * @param mesh - BJS mesh.
   * @returns - Boolean specifying if alpha blending is needed for the mesh.
   */
  virtual bool needAlphaBlendingForMesh(AbstractMesh* mesh) const;

  /**
   * @brief Specifies if this material should be rendered in alpha test mode.
   * @returns - Boolean specifying if an alpha test is needed.
   */
  virtual bool needAlphaTesting() const;

  /**
   * @brief Gets the texture used for the alpha test.
   * @returns - Nullable alpha test texture.
   */
  virtual BaseTexture* getAlphaTestTexture();

  /**
   * @brief Marks the material to indicate that it needs to be re-calculated.
   */
  void markDirty();

  bool _preBind(Effect* effect                             = nullptr,
                Nullable<unsigned int> overrideOrientation = nullptr);

  /**
   * @brief Binds the material to the mesh.
   * @param world - World transformation matrix.
   * @param mesh - Mesh to bind the material to.
   */
  virtual void bind(Matrix* world, Mesh* mesh);

  /**
   * @brief Binds the submesh to the material.
   * @param world - World transformation matrix.
   * @param mesh - Mesh containing the submesh.
   * @param subMesh - Submesh to bind the material to.
   */
  virtual void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh);

  /**
   * @brief Binds the world matrix to the material.
   * @param world - World transformation matrix.
   */
  virtual void bindOnlyWorldMatrix(Matrix& world);

  /**
   * @brief Binds the scene's uniform buffer to the effect.
   * @param effect - Effect to bind to the scene uniform buffer.
   * @param sceneUbo - Scene uniform buffer.
   */
  void bindSceneUniformBuffer(Effect* effect, UniformBuffer* sceneUbo);

  /**
   * @brief Binds the view matrix to the effect.
   * @param effect - Effect to bind the view matrix to.
   */
  void bindView(Effect* effect);

  /**
   * @brief Binds the view projection matrix to the effect.
   * @param effect - Effect to bind the view projection matrix to.
   */
  void bindViewProjection(Effect* effect);

  /**
   * @brief Unbinds the material from the mesh.
   */
  virtual void unbind();

  /**
   * @brief Gets the active textures from the material.
   * @returns - Array of textures.
   */
  virtual vector_t<BaseTexture*> getActiveTextures() const;

  /**
   * @brief Specifies if the material uses a texture.
   * @param texture - Texture to check against the material.
   * @returns - Boolean specifying if the material uses the texture.
   */
  virtual bool hasTexture(BaseTexture* texture) const;

  /**
   * @brief akes a duplicate of the material, and gives it a new name.
   * @param name - Name to call the duplicated material.
   * @returns - Nullable cloned material
   */
  virtual Material* clone(const string_t& name,
                          bool cloneChildren = false) const;

  /**
   * @brief Gets the meshes bound to the material.
   * @returns - Array of meshes bound to the material.
   */
  vector_t<AbstractMesh*> getBindedMeshes();

  /**
   * @brief Force shader compilation
   * @param mesh - BJS mesh.
   * @param onCompiled - function to execute once the material is compiled.
   * @param options - options to pass to this function.
   */
  void
  forceCompilation(AbstractMesh* mesh,
                   const ::std::function<void(Material* material)>& onCompiled,
                   Nullable<bool> clipPlane = false);

  /**
   * @brief Marks a define in the material to indicate that it needs to be
   * re-computed.
   * @param flag - Material define flag.
   */
  virtual void markAsDirty(unsigned int flag) override;

  /**
   * @brief Disposes the material.
   * @param forceDisposeEffect - Specifies if effects should be force disposed.
   * @param forceDisposeTextures - Specifies if textures should be force
   * disposed.
   */
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false);

  /**
   * Serializes this material.
   * @returns - serialized material object.
   */
  Json::object serialize() const;

  virtual void trackCreation(
    const ::std::function<void(const Effect* effect)>& onCompiled,
    const ::std::function<void(const Effect* effect, const string_t& errors)>&
      onError);

  void addMaterialToScene(unique_ptr_t<Material>&& newMaterial);
  void addMultiMaterialToScene(unique_ptr_t<MultiMaterial>&& newMultiMaterial);
  virtual bool useLogarithmicDepth() const;
  virtual void setUseLogarithmicDepth(bool value);
  virtual void setAmbientColor(const Color3& color);
  virtual void setDiffuseColor(const Color3& color);
  virtual void setSpecularColor(const Color3& color);
  virtual void setEmissiveColor(const Color3& color);
  virtual vector_t<Animation*> getAnimations() override;
  void copyTo(Material* other) const;

  /**
   * @brief Creates a MultiMaterial from parse MultiMaterial data.
   * @param parsedMultiMaterial - Parsed MultiMaterial data.
   * @param scene - BJS scene.
   * @returns - MultiMaterial.
   */
  static MultiMaterial*
  ParseMultiMaterial(const Json::value& parsedMultiMaterial, Scene* scene);

  /**
   * @brief Creates a material from parsed material data.
   * @param parsedMaterial - Parsed material data.
   * @param scene - BJS scene.
   * @param rootUrl - Root URL containing the material information.
   * @returns - Parsed material.
   */
  static Material* Parse(const Json::value& parsedMaterial, Scene* scene,
                         const string_t& rootUrl);

protected:
  /**
   * @brief Creates a material instance.
   * @param name - The name of the material.
   * @param scene - The BJS scene to reference.
   * @param doNotAdd - Specifies if the material should be added to the scene.
   */
  Material(const string_t& name, Scene* scene, bool doNotAdd = false);

  /**
   * @brief Specifies if material alpha testing should be turned on for the
   * mesh.
   * @param mesh - BJS mesh.
   */
  bool _shouldTurnAlphaTestOn(AbstractMesh* mesh) const;

  /**
   * @brief Processes to execute after binding the material to a mesh.
   * @param mesh - BJS mesh.
   */
  void _afterBind(Mesh* mesh);

  /**
   * @brief Marks all submeshes of a material to indicate that their material
   * defines need to be re-calculated.
   * @param func - function which checks material defines against the submeshes.
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
   * An event triggered when the material is disposed.
   */
  Observable<Material> onDisposeObservable;
  /**
   * An event triggered when the material is bound.
   */
  Observable<AbstractMesh> onBindObservable;
  /**
   * An event triggered when the material is unbound.
   */
  Observable<Material> onUnBindObservable;

  /**
   * The ID of the material.
   */
  string_t id;

  /**
   * The name of the material.
   */
  string_t name;

  /**
   * Specifies if the ready state should be checked on each call.
   */
  bool checkReadyOnEveryCall;

  /**
   * Specifies if the ready state should be checked once.
   */
  bool checkReadyOnlyOnce;

  /**
   * Stores the value for side orientation.
   */
  int sideOrientation;

  /**
   * Callback triggered when the material is compiled.
   */
  ::std::function<void(const Effect* effect)> onCompiled;

  /**
   * Callback triggered when an error occurs.
   */
  ::std::function<void(const Effect* effect, const string_t& errors)> onError;

  /**
   * Callback triggered to get the render target textures.
   */
  ::std::function<vector_t<RenderTargetTexture*>()> getRenderTargetTextures;

  /**
   * Specifies if the material should be serialized.
   */
  bool doNotSerialize;

  /**
   * Specifies if the effect should be stored on sub meshes.
   */
  bool storeEffectOnSubMeshes;

  /**
   * Stores the animations for the material.
   */
  vector_t<Animation*> animations;

  /**
   * Specifies if depth writing should be disabled.
   */
  bool disableDepthWrite;

  /**
   * Specifies if depth writing should be forced.
   */
  bool forceDepthWrite;

  /**
   * Specifies if there should be a separate pass for culling.
   */
  bool separateCullingPass;

  /**
   * Stores the size of points.
   */
  float pointSize;

  /**
   * Stores the z offset value.
   */
  float zOffset;

  /**
   * Stores the effects for the material.
   */
  Effect* _effect;

  /**
   * Specifies if the material was previously ready.
   */
  bool _wasPreviouslyReady;

protected:
  /**
   * The alpha value of the material.
   */
  float _alpha;

  /**
   * Specifies if back face culling is enabled.
   */
  bool _backFaceCulling;

  /**
   * Stores the uniform buffer.
   */
  unique_ptr_t<UniformBuffer> _uniformBuffer;

private:
  /**
   * An observer which watches for dispose events.
   */
  Observer<Material>::Ptr _onDisposeObserver;

  /**
   * An observer which watches for bind events.
   */
  Observer<AbstractMesh>::Ptr _onBindObserver;
  // Callbacks
  ::std::function<void()> _beforeRenderCallback;

  /**
   * Stores the value of the alpha mode.
   */
  unsigned int _alphaMode;

  /**
   * Stores the state of the need depth pre-pass value.
   */
  bool _needDepthPrePass;

  /**
   * Stores the state specifing if fog should be enabled.
   */
  bool _fogEnabled;

  /**
   * Specifies if uniform buffers should be used.
   */
  bool _useUBO;

  /**
   * Stores a reference to the scene.
   */
  Scene* _scene;

  /**
   * Stores the fill mode state.
   */
  unsigned int _fillMode;

  /**
   * Specifies if the depth write state should be cached.
   */
  bool _cachedDepthWriteState;

}; // end of class Material

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_H
