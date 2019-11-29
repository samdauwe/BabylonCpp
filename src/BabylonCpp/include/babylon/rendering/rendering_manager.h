#ifndef BABYLON_RENDERING_RENDERING_MANAGER_H
#define BABYLON_RENDERING_RENDERING_MANAGER_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/maths/color4.h>

namespace BABYLON {

class AbstractMesh;
class IParticleSystem;
struct IRenderingManagerAutoClearSetup;
struct ISpriteManager;
class Material;
class RenderingGroup;
struct RenderingGroupInfo;
class Scene;
class SpriteManager;
class SubMesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using MaterialPtr     = std::shared_ptr<Material>;
using SubMeshPtr      = std::shared_ptr<SubMesh>;

/**
 * @brief This is the manager responsible of all the rendering for meshes
 * sprites and particles. It is enable to manage the different groups as well as
 * the different necessary sort functions. This should not be used directly
 * aside of the few static configurations
 */
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
  /**
   * @brief Instantiates a new rendering group for a particular scene.
   * @param scene Defines the scene the groups belongs to
   */
  RenderingManager(Scene* scene);
  ~RenderingManager(); // = default

  /**
   * @brief Renders the entire managed groups. This is used by the scene or the
   * different rennder targets.
   * Hidden
   */
  void
  render(std::function<void(const std::vector<SubMesh*>& opaqueSubMeshes,
                            const std::vector<SubMesh*>& alphaTestSubMeshes,
                            const std::vector<SubMesh*>& transparentSubMeshes,
                            const std::vector<SubMesh*>& depthOnlySubMeshes,
                            const std::function<void()>& beforeTransparents)>
           customRenderFunction,
         const std::vector<AbstractMesh*>& activeMeshes, bool renderParticles,
         bool renderSprites);

  /**
   * @brief Resets the different information of the group to prepare a new frame
   * Hidden
   */
  void reset();

  /**
   * @brief Dispose and release the group and its associated resources.
   * Hidden
   */
  void dispose();

  /**
   * @brief Clear the info related to rendering groups preventing retention
   * points during dispose.
   */
  void freeRenderingGroups();

  /**
   * @brief Add a sprite manager to the rendering manager in order to render it
   * this frame.
   * @param spriteManager Define the sprite manager to render
   */
  void dispatchSprites(ISpriteManager* spriteManager);

  /**
   * @brief Add a particle system to the rendering manager in order to render it
   * this frame.
   * @param particleSystem Define the particle system to render
   */
  void dispatchParticles(IParticleSystem* particleSystem);

  /**
   * @brief Add a submesh to the manager in order to render it this frame.
   * @param subMesh The submesh to dispatch
   * @param mesh Optional reference to the submeshes's mesh. Provide if you have
   * an exiting reference to improve performance.
   * @param material Optional reference to the submeshes's material. Provide if
   * you have an exiting reference to improve performance.
   */
  void dispatch(SubMesh* subMesh, AbstractMesh* mesh = nullptr,
                const MaterialPtr& material = nullptr);

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
    const std::function<int(const SubMesh* a, const SubMesh* b)>&
      opaqueSortCompareFn
    = nullptr,
    const std::function<int(const SubMesh* a, const SubMesh* b)>&
      alphaTestSortCompareFn
    = nullptr,
    const std::function<int(const SubMesh* a, const SubMesh* b)>&
      transparentSortCompareFn
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

  /**
   * @brief Gets the current auto clear configuration for one rendering group of
   * the rendering manager.
   * @param index the rendering group index to get the information for
   * @returns The auto clear setup for the requested rendering group
   */
  std::optional<IRenderingManagerAutoClearSetup>
  getAutoClearDepthStencilSetup(size_t index);

private:
  void _clearDepthStencilBuffer(bool depth = true, bool stencil = true);
  void _prepareRenderingGroup(unsigned int renderingGroupId);

public:
  /**
   * Hidden
   */
  bool _useSceneAutoClearSetup;

private:
  Scene* _scene;
  std::vector<std::unique_ptr<RenderingGroup>> _renderingGroups;
  bool _depthStencilBufferAlreadyCleaned;

  std::vector<IRenderingManagerAutoClearSetup> _autoClearDepthStencil;
  std::vector<std::function<int(const SubMesh* a, const SubMesh* b)>>
    _customOpaqueSortCompareFn;
  std::vector<std::function<int(const SubMesh* a, const SubMesh* b)>>
    _customAlphaTestSortCompareFn;
  std::vector<std::function<int(const SubMesh* a, const SubMesh* b)>>
    _customTransparentSortCompareFn;
  std::unique_ptr<RenderingGroupInfo> _renderingGroupInfo;

}; // end of class RenderingManager

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_RENDERING_MANAGER_H
