#ifndef BABYLON_MATERIALS_MATERIAL_H
#define BABYLON_MATERIALS_MATERIAL_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Material : public IAnimatable {

public:
  static constexpr unsigned int TriangleFillMode  = 0;
  static constexpr unsigned int WireFrameFillMode = 1;
  static constexpr unsigned int PointFillMode     = 2;

  static constexpr unsigned int ClockWiseSideOrientation        = 0;
  static constexpr unsigned int CounterClockWiseSideOrientation = 1;

  static constexpr unsigned int TextureDirtyFlag    = 0;
  static constexpr unsigned int LightDirtyFlag      = 1;
  static constexpr unsigned int FresnelDirtyFlag    = 2;
  static constexpr unsigned int AttributesDirtyFlag = 4;
  static constexpr unsigned int MiscDirtyFlag       = 8;

  virtual ~Material();

  /**
   * @brief Returns the string "Material".
   */
  const char* getClassName() const;

  virtual IReflect::Type type() const override;
  void addMaterialToScene(std::unique_ptr<Material>&& newMaterial);
  void
  addMultiMaterialToScene(std::unique_ptr<MultiMaterial>&& newMultiMaterial);

  virtual void setAmbientColor(const Color3& color);
  virtual void setDiffuseColor(const Color3& color);
  virtual void setSpecularColor(const Color3& color);
  virtual void setEmissiveColor(const Color3& color);

  virtual std::vector<Animation*> getAnimations() override;

  // Events
  void setOnDispose(const std::function<void()>& callback);
  void setOnBind(const std::function<void()>& callback);

  bool wireframe() const;
  void setWireframe(bool value);
  bool pointsCloud() const;
  void setPointsCloud(bool value);
  unsigned int fillMode() const;
  void setFillMode(unsigned int value);

  /**
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading
   * subclasses should override adding information pertainent to themselves
   */
  std::string toString(bool fullDetails = false) const;

  bool isFrozen() const;
  void freeze();
  void unfreeze();
  virtual bool isReady(AbstractMesh* mesh = nullptr, bool useInstances = false);
  virtual bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh,
                                 bool useInstances = false);
  Effect* getEffect();
  Scene* getScene() const;
  virtual bool needAlphaBlending();
  virtual bool needAlphaTesting();
  virtual BaseTexture* getAlphaTestTexture();
  virtual void trackCreation(
    const std::function<void(const Effect* effect)>& onCompiled,
    const std::function<void(const Effect* effect, const std::string& errors)>&
      onError);
  void markDirty();
  void _preBind(Effect* effect = nullptr);
  virtual void bind(Matrix* world, Mesh* mesh);
  virtual void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh);
  virtual void bindOnlyWorldMatrix(Matrix& world);
  virtual void unbind();
  virtual Material* clone(const std::string& name,
                          bool cloneChildren = false) const;
  std::vector<AbstractMesh*> getBindedMeshes();
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false);
  void copyTo(Material* other) const;
  Json::object serialize() const;

  // Statics
  static MultiMaterial*
  ParseMultiMaterial(const Json::value& parsedMultiMaterial, Scene* scene);
  static Material* Parse(const Json::value& parsedMaterial, Scene* scene,
                         const std::string& rootUrl);

protected:
  Material(const std::string& name, Scene* scene, bool doNotAdd = false);

  void _afterBind(Mesh* mesh);

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
  // Properties
  std::string id;
  std::string name;
  bool checkReadyOnEveryCall;
  bool checkReadyOnlyOnce;
  std::string state;
  float alpha;
  bool backFaceCulling;
  int sideOrientation;
  std::function<void(const Effect* effect)> onCompiled;
  std::function<void(const Effect* effect, const std::string& errors)> onError;
  std::function<std::vector<RenderTargetTexture*>()> getRenderTargetTextures;
  bool doNotSerialize;
  bool storeEffectOnSubMeshes;
  int alphaMode;
  bool disableDepthWrite;
  bool fogEnabled;
  float pointSize;
  float zOffset;
  Effect* _effect;
  bool _wasPreviouslyReady;

private:
  // Events
  Observer<Material>::Ptr _onDisposeObserver;
  Observer<AbstractMesh>::Ptr _onBindObserver;
  // Properties
  Scene* _scene;
  unsigned int _fillMode;
  bool _cachedDepthWriteState;

}; // end of class Material

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_H
