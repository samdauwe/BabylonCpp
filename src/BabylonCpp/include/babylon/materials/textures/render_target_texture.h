#ifndef BABYLON_MATERIALS_TEXTURES_RENDER_TARGET_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_RENDER_TARGET_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT RenderTargetTexture : public Texture {

public:
  static constexpr unsigned int REFRESHRATE_RENDER_ONCE             = 0;
  static constexpr unsigned int REFRESHRATE_RENDER_ONEVERYFRAME     = 1;
  static constexpr unsigned int REFRESHRATE_RENDER_ONEVERYTWOFRAMES = 2;

public:
  RenderTargetTexture(
    const std::string& name, const ISize& size, Scene* scene,
    bool generateMipMaps = false, bool doNotChangeAspectRatio = true,
    unsigned int type         = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
    bool isCube               = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    bool generateDepthBuffer = true, bool generateStencilBuffer = false,
    bool isMulti = false);
  ~RenderTargetTexture();

  /** Events **/
  void setOnAfterUnbind(const std::function<void()>& callback);
  void setOnBeforeRender(const std::function<void(int faceIndex)>& callback);
  void setOnAfterRender(const std::function<void(int faceIndex)>& callback);
  void setOnClear(const std::function<void(Engine* engine)>& callback);

  IRenderTargetOptions& renderTargetOptions();
  const IRenderTargetOptions& renderTargetOptions() const;
  unsigned int samples() const;
  void setSamples(unsigned int value);
  void resetRefreshCounter();
  int refreshRate() const;
  void setRefreshRate(int value);
  bool _shouldRender();
  bool isReady() override;
  ISize& getRenderSize();
  bool canRescale() const;
  void scale(float ratio) override;
  Matrix* getReflectionTextureMatrix() override;
  void resize(const ISize& size);
  void render(bool useCameraPostProcess = false, bool dumpForDebug = false);
  void renderToTarget(unsigned int faceIndex,
                      const std::vector<AbstractMesh*>& currentRenderList,
                      size_t currentRenderListLength, bool useCameraPostProcess,
                      bool dumpForDebug);
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
    const std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn
    = nullptr,
    const std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn
    = nullptr,
    const std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn
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
  std::unique_ptr<RenderTargetTexture> clone() const;
  Json::object serialize() const;
  void dispose(bool doNotRecurse = false) override;

public:
  /**
   * Use this predicate to dynamically define the list of mesh you want to
   * render.
   * If set, the renderList property will be overwritten.
   */
  std::function<bool(AbstractMesh*)> renderListPredicate;

  /**
   * Use this list to define the list of mesh you want to render.
   */
  std::vector<AbstractMesh*> renderList;

  bool renderParticles;
  bool renderSprites;
  unsigned int coordinatesMode;
  Camera* activeCamera;
  std::function<void(const std::vector<SubMesh*>& opaqueSubMeshes,
                     const std::vector<SubMesh*>& transparentSubMeshes,
                     const std::vector<SubMesh*>& alphaTestSubMeshes)>
    customRenderFunction;
  bool _generateMipMaps;
  std::vector<std::string> _waitingRenderList;
  std::function<void()> onAfterRender;
  std::function<void()> onBeforeRender;

  // Events
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
  IRenderTargetOptions _renderTargetOptions;
  ISize _size;
  std::unique_ptr<RenderingManager> _renderingManager;
  bool _doNotChangeAspectRatio;
  int _currentRefreshId;
  int _refreshRate;
  std::unique_ptr<Matrix> _textureMatrix;
  unsigned int _samples;

private:
  // Events
  Observer<RenderTargetTexture>::Ptr _onAfterUnbindObserver;
  Observer<int>::Ptr _onBeforeRenderObserver;
  Observer<int>::Ptr _onAfterRenderObserver;
  Observer<Engine>::Ptr _onClearObserver;
  // Properties
  int _faceIndex;

}; // end of class RenderTargetTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_RENDER_TARGET_TEXTURE_H
