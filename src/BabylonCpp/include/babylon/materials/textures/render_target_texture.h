#ifndef BABYLON_MATERIALS_TEXTURES_RENDER_TARGET_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_RENDER_TARGET_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT RenderTargetTexture : public Texture {

private:
  static constexpr unsigned int _REFRESHRATE_RENDER_ONCE             = 0;
  static constexpr unsigned int _REFRESHRATE_RENDER_ONEVERYFRAME     = 1;
  static constexpr unsigned int _REFRESHRATE_RENDER_ONEVERYTWOFRAMES = 2;

public:
  static constexpr unsigned int REFRESHRATE_RENDER_ONCE()
  {
    return RenderTargetTexture::_REFRESHRATE_RENDER_ONCE;
  }

  static constexpr unsigned int REFRESHRATE_RENDER_ONEVERYFRAME()
  {
    return RenderTargetTexture::_REFRESHRATE_RENDER_ONEVERYFRAME;
  }

  static constexpr unsigned int REFRESHRATE_RENDER_ONEVERYTWOFRAMES()
  {
    return RenderTargetTexture::_REFRESHRATE_RENDER_ONEVERYTWOFRAMES;
  }

public:
  RenderTargetTexture(
    const string_t& name, const ISize& size, Scene* scene,
    bool generateMipMaps = false, bool doNotChangeAspectRatio = true,
    unsigned int type         = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
    bool isCube               = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    bool generateDepthBuffer = true, bool generateStencilBuffer = false,
    bool isMulti = false);
  ~RenderTargetTexture() override;

  void _onRatioRescale();

  /**
   * @brief Gets or sets the size of the bounding box associated with the
   * texture (when in cube mode) When defined, the cubemap will switch to local
   * mode
   * @see
   * https://community.arm.com/graphics/b/blog/posts/reflections-based-on-local-cubemaps-in-unity
   * Example: https://www.babylonjs-playground.com/#RNASML
   */
  void setBoundingBoxSize(const Vector3& value);
  Vector3* boundingBoxSize() const override;

  /** Events **/
  void setOnAfterUnbind(
    const ::std::function<void(RenderTargetTexture*, EventState&)>& callback);
  void setOnBeforeRender(
    const ::std::function<void(int* faceIndex, EventState&)>& callback);
  void setOnAfterRender(
    const ::std::function<void(int* faceIndex, EventState&)>& callback);
  void setOnClear(
    const ::std::function<void(Engine* engine, EventState&)>& callback);

  RenderTargetCreationOptions& renderTargetOptions();
  const RenderTargetCreationOptions& renderTargetOptions() const;
  unsigned int samples() const;
  void setSamples(unsigned int value);
  void resetRefreshCounter();
  int refreshRate() const;
  void setRefreshRate(int value);
  void addPostProcess(PostProcess* postProcess);
  void clearPostProcesses(bool dispose = false);
  void removePostProcess(PostProcess* postProcess);
  bool _shouldRender();
  bool isReady() override;
  ISize& getRenderSize();
  int getRenderWidth() const;
  int getRenderHeight() const;
  bool canRescale() const;
  void scale(float ratio) override;
  Matrix* getReflectionTextureMatrix() override;
  void resize(const ISize& size);
  void render(bool useCameraPostProcess = false, bool dumpForDebug = false);

  /**
   * @brief Overrides the default sort function applied in the renderging group
   * to prepare the meshes.
   * This allowed control for front to back rendering or reversly depending of
   * the special needs.   *
   * @param renderingGroupId The rendering group id corresponding to its index
   * @param opaqueSortCompareFn The opaque queue comparison function use to
   * sort.
   * @param alphaTestSortCompareFn The alpha test queue comparison function use
   * to sort.
   * @param transparentSortCompareFn The transparent queue comparison function
   * use to sort.
   */
  void setRenderingOrder(
    unsigned int renderingGroupId,
    const ::std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn
    = nullptr,
    const ::std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn
    = nullptr,
    const ::std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn
    = nullptr);

  /**
   * @brief Specifies whether or not the stencil and depth buffer are cleared
   * between two rendering groups.
   * @param renderingGroupId The rendering group id corresponding to its index
   * @param autoClearDepthStencil Automatically clears depth and stencil between
   * groups if true.
   */
  void setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                         bool autoClearDepthStencil);
  unique_ptr_t<RenderTargetTexture> clone() const;
  Json::object serialize() const;

  /**
   * @brief This will remove the attached framebuffer objects. The texture will
   * not be able to be used as render target anymore.
   */
  void disposeFramebufferObjects();

  void dispose(bool doNotRecurse = false) override;
  void _rebuild() override;

protected:
  void unbindFrameBuffer(Engine* engine, unsigned int faceIndex);

private:
  void _processSizeParameter(const ISize& size);
  int _bestReflectionRenderTargetDimension(int renderDimension,
                                           float scale) const;
  void renderToTarget(unsigned int faceIndex,
                      const vector_t<AbstractMesh*>& currentRenderList,
                      size_t currentRenderListLength, bool useCameraPostProcess,
                      bool dumpForDebug);

public:
  /**
   * Use this predicate to dynamically define the list of mesh you want to
   * render.
   * If set, the renderList property will be overwritten.
   */
  ::std::function<bool(AbstractMesh*)> renderListPredicate;

  /**
   * Use this list to define the list of mesh you want to render.
   */
  vector_t<AbstractMesh*> renderList;

  bool renderParticles;
  bool renderSprites;
  unsigned int coordinatesMode;
  Camera* activeCamera;
  ::std::function<void(const vector_t<SubMesh*>& opaqueSubMeshes,
                       const vector_t<SubMesh*>& alphaTestSubMeshes,
                       const vector_t<SubMesh*>& transparentSubMeshes,
                       const vector_t<SubMesh*>& depthOnlySubMeshes,
                       const ::std::function<void()>& beforeTransparents)>
    customRenderFunction;
  bool useCameraPostProcesses;
  bool ignoreCameraViewport;
  Nullable<Color4> clearColor;
  bool _generateMipMaps;
  vector_t<string_t> _waitingRenderList;
  ::std::function<void()> onAfterRender;
  ::std::function<void()> onBeforeRender;

  /**
   * Gets or sets the center of the bounding box associated with the texture
   * (when in cube mode) It must define where the camera used to render the
   * texture is set
   */
  Vector3 boundingBoxPosition;

  // Events

  /**
   * An event triggered when the texture is unbind.
   */
  Observable<RenderTargetTexture> onBeforeBindObservable;

  /**
   * An event triggered when the texture is unbind.
   */
  Observable<RenderTargetTexture> onAfterUnbindObservable;

  /**
   * An event triggered before rendering the texture
   */
  Observable<int> onBeforeRenderObservable;

  /**
   * An event triggered after rendering the texture
   */
  Observable<int> onAfterRenderObservable;

  /**
   * An event triggered after the texture clear
   */
  Observable<Engine> onClearObservable;

protected:
  RenderTargetCreationOptions _renderTargetOptions;
  ISize _size;
  ISize _initialSizeParameter;
  float _sizeRatio;
  unique_ptr_t<RenderingManager> _renderingManager;
  bool _doNotChangeAspectRatio;
  int _currentRefreshId;
  int _refreshRate;
  unique_ptr_t<Matrix> _textureMatrix;
  unsigned int _samples;
  Engine* _engine;

private:
  unique_ptr_t<PostProcessManager> _postProcessManager;
  vector_t<PostProcess*> _postProcesses;
  Observer<Engine>::Ptr _resizeObserver;
  // Events
  Observer<RenderTargetTexture>::Ptr _onAfterUnbindObserver;
  Observer<int>::Ptr _onBeforeRenderObserver;
  Observer<int>::Ptr _onAfterRenderObserver;
  Observer<Engine>::Ptr _onClearObserver;
  // Properties
  int _faceIndex;
  unique_ptr_t<Vector3> _boundingBoxSize;

}; // end of class RenderTargetTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_RENDER_TARGET_TEXTURE_H
