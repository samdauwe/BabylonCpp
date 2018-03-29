#ifndef BABYLON_RENDERING_RENDERING_MANAGER_H
#define BABYLON_RENDERING_RENDERING_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/math/color4.h>

namespace BABYLON {

/**
 * @brief Interface describing the different options available in the rendering
 * manager regarding Auto Clear between groups.
 */
struct BABYLON_SHARED_EXPORT RenderingManageAutoClearOptions {
  bool autoClear;
  bool depth;
  bool stencil;
}; // end of struct RenderingManageAutoClearOptions

class BABYLON_SHARED_EXPORT RenderingManager {

public:
  /**
   * The max id used for rendering groups (not included)
   */
  static constexpr unsigned int MAX_RENDERINGGROUPS = 4;

  /**
   * The min id used for rendering groups (included)
   */
  static constexpr unsigned int MIN_RENDERINGGROUPS = 0;

  /**
   * Used to globally prevent autoclearing scenes.
   */
  static bool AUTOCLEAR;

public:
  RenderingManager(Scene* scene);
  ~RenderingManager();

  void render(
    ::std::function<void(const vector_t<SubMesh*>& opaqueSubMeshes,
                         const vector_t<SubMesh*>& alphaTestSubMeshes,
                         const vector_t<SubMesh*>& transparentSubMeshes,
                         const vector_t<SubMesh*>& depthOnlySubMeshes,
                         const ::std::function<void()>& beforeTransparents)>
      customRenderFunction,
    const vector_t<AbstractMesh*>& activeMeshes, bool renderParticles,
    bool renderSprites);
  void reset();
  void dispose();

  /**
   * @brief Clear the info related to rendering groups preventing retention
   * points during dispose.
   */
  void freeRenderingGroups();

  void dispatchSprites(SpriteManager* spriteManager);
  void dispatchParticles(IParticleSystem* particleSystem);

  /**
   * @param subMesh The submesh to dispatch
   * @param [mesh] Optional reference to the submeshes's mesh. Provide if you
   * have an exiting reference to improve performance.
   * @param [material] Optional reference to the submeshes's material. Provide
   * if you have an exiting reference to improve performance.
   */
  void dispatch(SubMesh* subMesh, AbstractMesh* mesh = nullptr,
                Material* material = nullptr);

  /**
   * @brief Overrides the default sort function applied in the renderging group
   * to prepare the meshes.
   * This allowed control for front to back rendering or reversly depending of
   * the special needs.
   *
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
   *
   * @param renderingGroupId The rendering group id corresponding to its index
   * @param autoClearDepthStencil Automatically clears depth and stencil between
   * groups if true.
   * @param depth Automatically clears depth between groups if true and
   * autoClear is true.
   * @param stencil Automatically clears stencil between groups if true and
   * autoClear is true.
   */
  void setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                         bool autoClearDepthStencil,
                                         bool depth   = true,
                                         bool stencil = true);

private:
  void _clearDepthStencilBuffer(bool depth = true, bool stencil = true);
  void _prepareRenderingGroup(unsigned int renderingGroupId);

private:
  Scene* _scene;
  vector_t<unique_ptr_t<RenderingGroup>> _renderingGroups;
  bool _depthStencilBufferAlreadyCleaned;

  vector_t<RenderingManageAutoClearOptions> _autoClearDepthStencil;
  vector_t<::std::function<int(SubMesh* a, SubMesh* b)>>
    _customOpaqueSortCompareFn;
  vector_t<::std::function<int(SubMesh* a, SubMesh* b)>>
    _customAlphaTestSortCompareFn;
  vector_t<::std::function<int(SubMesh* a, SubMesh* b)>>
    _customTransparentSortCompareFn;
  unique_ptr_t<RenderingGroupInfo> _renderinGroupInfo;

}; // end of class RenderingManager

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_RENDERING_MANAGER_H
