#ifndef BABYLON_RENDERING_RENDERING_GROUP_H
#define BABYLON_RENDERING_RENDERING_GROUP_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class AbstractMesh;
class Camera;
class EdgesRenderer;
struct IEdgesRenderer;
class IParticleSystem;
struct ISpriteManager;
class Material;
class Scene;
class SubMesh;
using AbstractMeshPtr   = std::shared_ptr<AbstractMesh>;
using CameraPtr         = std::shared_ptr<Camera>;
using IEdgesRendererPtr = std::shared_ptr<IEdgesRenderer>;
using MaterialPtr       = std::shared_ptr<Material>;
using SubMeshPtr        = std::shared_ptr<SubMesh>;

/**
 * @brief This represents the object necessary to create a rendering group.
 * This is exclusively used and created by the rendering manager.
 * To modify the behavior, you use the available helpers in your scene or
 * meshes.
 * Hidden
 */
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
  RenderingGroup(unsigned int index, Scene* scene,
                 const std::function<bool(const SubMesh* a, const SubMesh* b)>&
                   opaqueSortCompareFn
                 = nullptr,
                 const std::function<bool(const SubMesh* a, const SubMesh* b)>&
                   alphaTestSortCompareFn
                 = nullptr,
                 const std::function<bool(const SubMesh* a, const SubMesh* b)>&
                   transparentSortCompareFn
                 = nullptr);
  ~RenderingGroup(); // = default

  /**
   * @brief Render all the sub meshes contained in the group.
   * @param customRenderFunction Used to override the default render behaviour
   * of the group.
   */
  void
  render(std::function<void(const std::vector<SubMesh*>& opaqueSubMeshes,
                            const std::vector<SubMesh*>& alphaTestSubMeshes,
                            const std::vector<SubMesh*>& transparentSubMeshes,
                            const std::vector<SubMesh*>& depthOnlySubMeshes,
                            const std::function<void()>& beforeTransparents)>&
           customRenderFunction,
         bool renderSprites, bool renderParticles,
         const std::vector<AbstractMesh*>& activeMeshes);

  /**
   * @brief Build in function which can be applied to ensure meshes of a special
   * queue (opaque, alpha test, transparent) are rendered back to front if in
   * the same alpha index.
   *
   * @param a The first submesh
   * @param b The second submesh
   * @returns The result of the comparison
   */
  static bool defaultTransparentSortCompare(const SubMesh* a, const SubMesh* b);

  /**
   * @brief Build in function which can be applied to ensure meshes of a special
   * queue (opaque, alpha test, transparent) are rendered back to front.
   *
   * @param a The first submesh
   * @param b The second submesh
   * @returns The result of the comparison
   */
  static bool backToFrontSortCompare(const SubMesh* a, const SubMesh* b);

  /**
   * @brief Build in function which can be applied to ensure meshes of a special
   * queue (opaque, alpha test, transparent) are rendered front to back (prevent
   * overdraw).
   *
   * @param a The first submesh
   * @param b The second submesh
   * @returns The result of the comparison
   */
  static bool frontToBackSortCompare(SubMesh* a, SubMesh* b);

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
  void dispatch(SubMesh* subMesh, AbstractMesh* mesh = nullptr,
                MaterialPtr material = nullptr);

  void dispatchSprites(ISpriteManager* spriteManager);

  void dispatchParticles(IParticleSystem* particleSystem);

private:
  /**
   * @brief Renders the opaque submeshes in the order from the
   * opaqueSortCompareFn.
   * @param subMeshes The submeshes to render
   */
  void renderOpaqueSorted(const std::vector<SubMesh*>& subMeshes);

  /**
   * @brief Renders the opaque submeshes in the order from the
   * alphatestSortCompareFn.
   * @param subMeshes The submeshes to render
   */
  void renderAlphaTestSorted(const std::vector<SubMesh*>& subMeshes);

  /**
   * @brief Renders the opaque submeshes in the order from the
   * transparentSortCompareFn.
   * @param subMeshes The submeshes to render
   */
  void renderTransparentSorted(const std::vector<SubMesh*>& subMeshes);

  void _renderParticles(const std::vector<AbstractMesh*>& activeMeshes);

  void _renderSprites();

  /**
   * @brief Renders the submeshes in a specified order.
   * @param subMeshes The submeshes to sort before render
   * @param sortCompareFn The comparison function use to sort
   * @param camera The camera to use to preprocess the submeshes to help sorting
   * @param transparent Specifies to activate blending if true
   */
  static void
  renderSorted(const std::vector<SubMesh*>& subMeshes,
               const std::function<bool(const SubMesh* a, const SubMesh* b)>&
                 sortCompareFn,
               const CameraPtr& camera, bool transparent);

  /**
   * @brief Renders the submeshes in the order they were dispatched (no sort
   * applied).
   * @param subMeshes The submeshes to render
   */
  static void renderUnsorted(const std::vector<SubMesh*>& subMeshes);

protected:
  /**
   * @brief Set the opaque sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  void set_opaqueSortCompareFn(
    const std::function<bool(const SubMesh* a, const SubMesh* b)>& value);

  /**
   * @brief Set the alpha test sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  void set_alphaTestSortCompareFn(
    const std::function<bool(const SubMesh* a, const SubMesh* b)>& value);

  /**
   * @brief Set the transparent sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  void set_transparentSortCompareFn(
    const std::function<bool(const SubMesh* a, const SubMesh* b)>& value);

public:
  /** Hidden */
  std::vector<IEdgesRendererPtr> _edgesRenderers;

  unsigned int index;
  std::function<void()> onBeforeTransparentRendering;

  /**
   * Sets the opaque sort comparison function
   * If null the sub meshes will be render in the order they were created
   */
  WriteOnlyProperty<RenderingGroup,
                    std::function<bool(const SubMesh* a, const SubMesh* b)>>
    opaqueSortCompareFn;

  /**
   * Sets the alpha test sort comparison function.
   * If null the sub meshes will be render in the order they were created
   */
  WriteOnlyProperty<RenderingGroup,
                    std::function<bool(const SubMesh* a, const SubMesh* b)>>
    alphaTestSortCompareFn;

  /**
   * Sets the transparent sort comparison function
   * If null the sub meshes will be render in the order they were created
   */
  WriteOnlyProperty<RenderingGroup,
                    std::function<bool(const SubMesh* a, const SubMesh* b)>>
    transparentSortCompareFn;

private:
  static Vector3 _zeroVector;
  Scene* _scene;
  std::vector<SubMesh*> _opaqueSubMeshes;
  std::vector<SubMesh*> _transparentSubMeshes;
  std::vector<SubMesh*> _alphaTestSubMeshes;
  std::vector<SubMesh*> _depthOnlySubMeshes;
  std::vector<IParticleSystem*> _particleSystems;
  std::vector<ISpriteManager*> _spriteManagers;

  std::function<bool(const SubMesh* a, const SubMesh* b)> _opaqueSortCompareFn;
  std::function<bool(const SubMesh* a, const SubMesh* b)>
    _alphaTestSortCompareFn;
  std::function<bool(const SubMesh* a, const SubMesh* b)>
    _transparentSortCompareFn;

  std::function<void(const std::vector<SubMesh*>& subMeshes)> _renderOpaque;
  std::function<void(const std::vector<SubMesh*>& subMeshes)> _renderAlphaTest;
  std::function<void(const std::vector<SubMesh*>& subMeshes)>
    _renderTransparent;

}; // end of class RenderingGroup

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_RENDERING_GROUP_H
