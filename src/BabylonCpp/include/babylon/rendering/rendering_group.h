#ifndef BABYLON_RENDERING_RENDERING_GROUP_H
#define BABYLON_RENDERING_RENDERING_GROUP_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT RenderingGroup {

public:
  /**
   * Creates a new rendering group.
   * @param index The rendering group index
   * @param opaqueSortCompareFn The opaque sort comparison function. If null no
   * order is applied
   * @param alphaTestSortCompareFn The alpha test sort comparison function. If
   * null no order is applied
   * @param transparentSortCompareFn The transparent sort comparison function.
   * If null back to front + alpha index sort is applied
   */
  RenderingGroup(
    unsigned int index, Scene* scene,
    const std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn
    = nullptr,
    const std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn
    = nullptr,
    const std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn
    = nullptr);
  ~RenderingGroup();

  /**
   * Set the opaque sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  void setOpaqueSortCompareFn(
    const std::function<int(SubMesh* a, SubMesh* b)>& value);

  /**
   * Set the alpha test sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  void setAlphaTestSortCompareFn(
    const std::function<int(SubMesh* a, SubMesh* b)>& value);

  /**
   * Set the transparent sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  void setTransparentSortCompareFn(
    const std::function<int(SubMesh* a, SubMesh* b)>& value);

  /**
   * Render all the sub meshes contained in the group.
   * @param customRenderFunction Used to override the default render behaviour
   * of the group.
   */
  void
  render(std::function<void(const std::vector<SubMesh*>& opaqueSubMeshes,
                            const std::vector<SubMesh*>& transparentSubMeshes,
                            const std::vector<SubMesh*>& alphaTestSubMeshes)>&
           customRenderFunction,
         bool renderSprites, bool renderParticles,
         const std::vector<AbstractMesh*> activeMeshes);

  /**
   * Build in function which can be applied to ensure meshes of a special queue
   * (opaque, alpha test, transparent)
   * are rendered back to front if in the same alpha index.
   *
   * @param a The first submesh
   * @param b The second submesh
   * @returns The result of the comparison
   */
  static int defaultTransparentSortCompare(SubMesh* a, SubMesh* b);

  /**
   * Build in function which can be applied to ensure meshes of a special queue
   * (opaque, alpha test, transparent)
   * are rendered back to front.
   *
   * @param a The first submesh
   * @param b The second submesh
   * @returns The result of the comparison
   */
  static int backToFrontSortCompare(SubMesh* a, SubMesh* b);

  /**
   * Build in function which can be applied to ensure meshes of a special queue
   * (opaque, alpha test, transparent)
   * are rendered front to back (prevent overdraw).
   *
   * @param a The first submesh
   * @param b The second submesh
   * @returns The result of the comparison
   */
  static int frontToBackSortCompare(SubMesh* a, SubMesh* b);

  /**
   * Resets the different lists of submeshes to prepare a new frame.
   */
  void prepare();

  void dispose();

  /**
   * Inserts the submesh in its correct queue depending on its material.
   * @param subMesh The submesh to dispatch
   */
  void dispatch(SubMesh* subMesh);

  void dispatchSprites(SpriteManager* spriteManager);

  void dispatchParticles(ParticleSystem* particleSystem);

private:
  /**
   * Renders the opaque submeshes in the order from the opaqueSortCompareFn.
   * @param subMeshes The submeshes to render
   */
  void renderOpaqueSorted(const std::vector<SubMesh*>& subMeshes);

  /**
   * Renders the opaque submeshes in the order from the alphatestSortCompareFn.
   * @param subMeshes The submeshes to render
   */
  void renderAlphaTestSorted(const std::vector<SubMesh*>& subMeshes);

  /**
   * Renders the opaque submeshes in the order from the
   * transparentSortCompareFn.
   * @param subMeshes The submeshes to render
   */
  void renderTransparentSorted(const std::vector<SubMesh*>& subMeshes);

  void _renderParticles(const std::vector<AbstractMesh*>& activeMeshes);

  void _renderSprites();

  /**
   * Renders the submeshes in a specified order.
   * @param subMeshes The submeshes to sort before render
   * @param sortCompareFn The comparison function use to sort
   * @param cameraPosition The camera position use to preprocess the submeshes
   * to help sorting
   * @param transparent Specifies to activate blending if true
   */
  static void
  renderSorted(const std::vector<SubMesh*>& subMeshes,
               const std::function<int(SubMesh* a, SubMesh* b)>& sortCompareFn,
               const Vector3& cameraPosition, bool transparent);

  /**
   * Renders the submeshes in the order they were dispatched (no sort applied).
   * @param subMeshes The submeshes to render
   */
  static void renderUnsorted(const std::vector<SubMesh*>& subMeshes);

public:
  unsigned int index;
  std::function<void()> onBeforeTransparentRendering;

private:
  Scene* _scene;
  std::vector<SubMesh*> _opaqueSubMeshes;
  std::vector<SubMesh*> _transparentSubMeshes;
  std::vector<SubMesh*> _alphaTestSubMeshes;
  std::vector<ParticleSystem*> _particleSystems;
  std::vector<SpriteManager*> _spriteManagers;
  size_t _activeVertices;

  std::function<int(SubMesh* a, SubMesh* b)> _opaqueSortCompareFn;
  std::function<int(SubMesh* a, SubMesh* b)> _alphaTestSortCompareFn;
  std::function<int(SubMesh* a, SubMesh* b)> _transparentSortCompareFn;

  std::function<void(const std::vector<SubMesh*>& subMeshes)> _renderOpaque;
  std::function<void(const std::vector<SubMesh*>& subMeshes)> _renderAlphaTest;
  std::function<void(const std::vector<SubMesh*>& subMeshes)>
    _renderTransparent;

  std::vector<EdgesRenderer*> _edgesRenderers;

}; // end of class RenderingGroup

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_RENDERING_GROUP_H
