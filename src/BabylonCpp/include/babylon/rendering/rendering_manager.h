#ifndef BABYLON_RENDERING_RENDERING_MANAGER_H
#define BABYLON_RENDERING_RENDERING_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/math/color4.h>

namespace BABYLON {

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

public:
  RenderingManager(Scene* scene);
  ~RenderingManager();

  void
  render(std::function<void(const std::vector<SubMesh*>& opaqueSubMeshes,
                            const std::vector<SubMesh*>& transparentSubMeshes,
                            const std::vector<SubMesh*>& alphaTestSubMeshes)>
           customRenderFunction,
         const std::vector<AbstractMesh*>& activeMeshes, bool renderParticles,
         bool renderSprites);
  void reset();
  void dispatch(SubMesh* subMesh);

  /**
   * Overrides the default sort function applied in the renderging group to
   * prepare the meshes.
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
    const std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn
    = nullptr,
    const std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn
    = nullptr,
    const std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn
    = nullptr);

  /**
   * Specifies whether or not the stencil and depth buffer are cleared between
   * two rendering groups.
   *
   * @param renderingGroupId The rendering group id corresponding to its index
   * @param autoClearDepthStencil Automatically clears depth and stencil between
   * groups if true.
   */
  void setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                         bool autoClearDepthStencil);

private:
  void _renderParticles(unsigned int index,
                        const std::vector<AbstractMesh*>& activeMeshes);
  void _renderSprites(unsigned int index);
  void _clearDepthStencilBuffer();
  void _renderSpritesAndParticles();

private:
  Scene* _scene;
  std::vector<std::unique_ptr<RenderingGroup>> _renderingGroups;
  bool _depthBufferAlreadyCleaned;

  unsigned int _currentIndex;
  std::vector<AbstractMesh*> _currentActiveMeshes;
  bool _currentRenderParticles;
  bool _currentRenderSprites;
  Color4 _clearColor;

  std::vector<bool> _autoClearDepthStencil;
  std::vector<std::function<int(SubMesh* a, SubMesh* b)>>
    _customOpaqueSortCompareFn;
  std::vector<std::function<int(SubMesh* a, SubMesh* b)>>
    _customAlphaTestSortCompareFn;
  std::vector<std::function<int(SubMesh* a, SubMesh* b)>>
    _customTransparentSortCompareFn;
  std::unique_ptr<RenderingGroupInfo> _renderinGroupInfo;

}; // end of class RenderingManager

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_RENDERING_MANAGER_H
