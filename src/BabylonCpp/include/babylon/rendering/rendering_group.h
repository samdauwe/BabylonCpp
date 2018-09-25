#ifndef BABYLON_RENDERING_RENDERING_GROUP_H
#define BABYLON_RENDERING_RENDERING_GROUP_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT RenderingGroup {

public:
  /**
   * @brief Creates a new rendering group.
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
    const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
      opaqueSortCompareFn
    = nullptr,
    const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
      alphaTestSortCompareFn
    = nullptr,
    const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
      transparentSortCompareFn
    = nullptr);
  ~RenderingGroup();

  /**
   * @brief Render all the sub meshes contained in the group.
   * @param customRenderFunction Used to override the default render behaviour
   * of the group.
   */
  void render(
    std::function<void(const vector_t<SubMeshPtr>& opaqueSubMeshes,
                         const vector_t<SubMeshPtr>& alphaTestSubMeshes,
                         const vector_t<SubMeshPtr>& transparentSubMeshes,
                         const vector_t<SubMeshPtr>& depthOnlySubMeshes,
                         const std::function<void()>& beforeTransparents)>&
      customRenderFunction,
    bool renderSprites, bool renderParticles,
    const vector_t<AbstractMeshPtr>& activeMeshes);

  /**
   * @brief Build in function which can be applied to ensure meshes of a special
   * queue (opaque, alpha test, transparent) are rendered back to front if in
   * the same alpha index.
   *
   * @param a The first submesh
   * @param b The second submesh
   * @returns The result of the comparison
   */
  static int defaultTransparentSortCompare(const SubMeshPtr& a,
                                           const SubMeshPtr& b);

  /**
   * @brief Build in function which can be applied to ensure meshes of a special
   * queue (opaque, alpha test, transparent) are rendered back to front.
   *
   * @param a The first submesh
   * @param b The second submesh
   * @returns The result of the comparison
   */
  static int backToFrontSortCompare(const SubMeshPtr& a, const SubMeshPtr& b);

  /**
   * @brief Build in function which can be applied to ensure meshes of a special
   * queue (opaque, alpha test, transparent) are rendered front to back (prevent
   * overdraw).
   *
   * @param a The first submesh
   * @param b The second submesh
   * @returns The result of the comparison
   */
  static int frontToBackSortCompare(SubMesh* a, SubMesh* b);

  /**
   * @brief Resets the different lists of submeshes to prepare a new frame.
   */
  void prepare();

  void dispose();

  /**
   * @brief Inserts the submesh in its correct queue depending on its material.
   * @param subMesh The submesh to dispatch
   * @param [mesh] Optional reference to the submeshes's mesh. Provide if you
   * have an exiting reference to improve performance.
   * @param [material] Optional reference to the submeshes's material. Provide
   * if you have an exiting reference to improve performance.
   */
  void dispatch(const SubMeshPtr& subMesh, AbstractMesh* mesh = nullptr,
                MaterialPtr material = nullptr);

  void dispatchSprites(SpriteManager* spriteManager);

  void dispatchParticles(IParticleSystem* particleSystem);

private:
  /**
   * @brief Renders the opaque submeshes in the order from the
   * opaqueSortCompareFn.
   * @param subMeshes The submeshes to render
   */
  void renderOpaqueSorted(const vector_t<SubMeshPtr>& subMeshes);

  /**
   * @brief Renders the opaque submeshes in the order from the
   * alphatestSortCompareFn.
   * @param subMeshes The submeshes to render
   */
  void renderAlphaTestSorted(const vector_t<SubMeshPtr>& subMeshes);

  /**
   * @brief Renders the opaque submeshes in the order from the
   * transparentSortCompareFn.
   * @param subMeshes The submeshes to render
   */
  void renderTransparentSorted(const vector_t<SubMeshPtr>& subMeshes);

  void _renderParticles(const vector_t<AbstractMeshPtr>& activeMeshes);

  void _renderSprites();

  /**
   * @brief Renders the submeshes in a specified order.
   * @param subMeshes The submeshes to sort before render
   * @param sortCompareFn The comparison function use to sort
   * @param camera The camera to use to preprocess the submeshes to help sorting
   * @param transparent Specifies to activate blending if true
   */
  static void renderSorted(
    const vector_t<SubMeshPtr>& subMeshes,
    const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
      sortCompareFn,
    const CameraPtr& camera, bool transparent);

  /**
   * @brief Renders the submeshes in the order they were dispatched (no sort
   * applied).
   * @param subMeshes The submeshes to render
   */
  static void renderUnsorted(const vector_t<SubMeshPtr>& subMeshes);

protected:
  /**
   * @brief Set the opaque sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  void set_opaqueSortCompareFn(
    const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
      value);

  /**
   * @brief Set the alpha test sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  void set_alphaTestSortCompareFn(
    const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
      value);

  /**
   * @brief Set the transparent sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  void set_transparentSortCompareFn(
    const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
      value);

public:
  unsigned int index;
  std::function<void()> onBeforeTransparentRendering;

  /**
   * Sets the opaque sort comparison function
   * If null the sub meshes will be render in the order they were created
   */
  WriteOnlyProperty<RenderingGroup, std::function<int(const SubMeshPtr& a,
                                                        const SubMeshPtr& b)>>
    opaqueSortCompareFn;

  /**
   * Sets the alpha test sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  WriteOnlyProperty<RenderingGroup, std::function<int(const SubMeshPtr& a,
                                                        const SubMeshPtr& b)>>
    alphaTestSortCompareFn;

  /**
   * Sets the transparent sort comparison function
   * If null the sub meshes will be render in the order they were created
   */
  WriteOnlyProperty<RenderingGroup, std::function<int(const SubMeshPtr& a,
                                                        const SubMeshPtr& b)>>
    transparentSortCompareFn;

private:
  Scene* _scene;
  vector_t<SubMeshPtr> _opaqueSubMeshes;
  vector_t<SubMeshPtr> _transparentSubMeshes;
  vector_t<SubMeshPtr> _alphaTestSubMeshes;
  vector_t<SubMeshPtr> _depthOnlySubMeshes;
  vector_t<IParticleSystem*> _particleSystems;
  vector_t<SpriteManager*> _spriteManagers;

  std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>
    _opaqueSortCompareFn;
  std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>
    _alphaTestSortCompareFn;
  std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>
    _transparentSortCompareFn;

  std::function<void(const vector_t<SubMeshPtr>& subMeshes)> _renderOpaque;
  std::function<void(const vector_t<SubMeshPtr>& subMeshes)> _renderAlphaTest;
  std::function<void(const vector_t<SubMeshPtr>& subMeshes)>
    _renderTransparent;

  vector_t<EdgesRenderer*> _edgesRenderers;

}; // end of class RenderingGroup

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_RENDERING_GROUP_H
