#include <babylon/materials/shadow_depth_wrapper.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/base_sub_mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

template <typename Ka, typename Kb, typename V>
class MapMap {

public:
  std::optional<V> get(const Ka& a, const Kb& b)
  {
    if (stl_util::contains(mm, a)) {
      const auto& m = mm[a];
      if (stl_util::contains(m, b)) {
        return m[b];
      }
    }
    return std::nullopt;
  }

  void set(const Ka& a, const Kb& b, const V& v)
  {
    if (!stl_util::contains(mm, a)) {
      mm[a] = std::unordered_map<Kb, V>();
    }
    mm[a][b] = v;
  }

public:
  std::unordered_map<Ka, std::unordered_map<Kb, V>> mm;
};

/**
 * @brief The actual ShadowDepthWrapper implementation definition.
 * @see http://oliora.github.io/2015/12/29/pimpl-and-rule-of-zero.html
 */
class ShadowDepthWrapper::ShadowDepthWrapperImpl {

public:
  ShadowDepthWrapperImpl(const MaterialPtr& baseMaterial, Scene* scene,
                         const std::optional<IIOptionShadowDepthMaterial>& options);
  ~ShadowDepthWrapperImpl();

  /**
   * @brief Gets the effect to use to generate the depth map
   * @param subMesh subMesh to get the effect for
   * @param shadowGenerator shadow generator to get the effect for
   * @returns the effect to use to generate the depth map for the subMesh + shadow generator
   * specified
   */
  Effect* getEffect(SubMesh* subMesh, ShadowGenerator* shadowGenerator);

  /**
   * @brief Specifies that the submesh is ready to be used for depth rendering
   * @param subMesh submesh to check
   * @param defines the list of defines to take into account when checking the effect
   * @param shadowGenerator combined with subMesh, it defines the effect to check
   * @param useInstances specifies that instances should be used
   * @returns a boolean indicating that the submesh is ready or not
   */
  bool isReadyForSubMesh(SubMesh* subMesh, const std::vector<std::string>& defines,
                         ShadowGenerator* shadowGenerator, bool useInstances);

  /**
   * @brief Disposes the resources
   */
  void dispose();

  /**
   * @brief Gets the standalone status of the wrapper.
   */
  bool get_standalone() const;

  /**
   * @brief Gets the base material the wrapper is built upon.
   */
  MaterialPtr& get_baseMaterial();

private:
  Effect* _makeEffect(SubMesh* subMesh, const std::vector<std::string>& defines,
                      ShadowGenerator* shadowGenerator);

public:
  /**
   * Hidden
   */
  std::unordered_map<std::string, std::string> _matriceNames;

private:
  Scene* _scene;
  std::optional<IIOptionShadowDepthMaterial> _options;
  MaterialPtr _baseMaterial;
  Observer<OnCreatedEffectParameters>::Ptr _onEffectCreatedObserver;
  std::unordered_map<SubMesh*, Effect> _subMeshToEffect;
  MapMap<SubMesh*, ShadowGenerator*, DepthEffect> _subMeshToDepthEffect;
  std::unordered_map<AbstractMeshPtr, Observer<Node>::Ptr> _meshes;

}; // end of class ShadowDepthWrapper::ShadowDepthWrapperImpl

ShadowDepthWrapper::ShadowDepthWrapperImpl::ShadowDepthWrapperImpl(
  const MaterialPtr& baseMaterial, Scene* scene,
  const std::optional<IIOptionShadowDepthMaterial>& options)
{
  _baseMaterial = baseMaterial;
  _scene        = scene;
  _options      = options;

  const std::string prefix = baseMaterial->getClassName() == "NodeMaterial" ? "u_" : "";

  _matriceNames = {
    {"view", prefix + "view"},                              //
    {"projection", prefix + "projection"},                  //
    {"viewProjection", prefix + "viewProjection"},          //
    {"worldView", prefix + "worldView"},                    //
    {"worldViewProjection", prefix + "worldViewProjection"} //
  };

  // Register for onEffectCreated to store the effect of the base material when it is (re)generated.
  // This effect will be used to create the depth effect later on
  _onEffectCreatedObserver = _baseMaterial->onEffectCreatedObservable().add(
    [this](OnCreatedEffectParameters* params, EventState & /*es*/) -> void {
      const auto mesh = static_cast<SubMesh*>(params->subMesh)->getMesh();

      if (mesh && !stl_util::contains(_meshes, mesh)) {
        // Register for mesh onDispose to clean up our internal maps when a mesh is disposed
        _meshes[mesh]
          = mesh->onDisposeObservable.add([this](Node* mesh, EventState & /*es*/) -> void {
              for (const auto& item : _subMeshToEffect) {
                const auto subMesh = item.first;
                if (subMesh->getMesh().get() == mesh) {
                  _subMeshToEffect.erase(subMesh);
                  _subMeshToDepthEffect.mm.erase(subMesh);
                }
              }
            });
      }

      // _subMeshToEffect.set(params->subMesh, params->effect);
      // _subMeshToDepthEffect.mm.erase(params->subMesh); // trigger a depth effect recreation
    });
}

ShadowDepthWrapper::ShadowDepthWrapperImpl::~ShadowDepthWrapperImpl() = default;

bool ShadowDepthWrapper::ShadowDepthWrapperImpl::get_standalone() const
{
  return _options ? _options->standalone.value_or(false) : false;
}

MaterialPtr& ShadowDepthWrapper::ShadowDepthWrapperImpl::get_baseMaterial()
{
  return _baseMaterial;
}

Effect* ShadowDepthWrapper::ShadowDepthWrapperImpl::getEffect(SubMesh* /*subMesh*/,
                                                              ShadowGenerator* /*shadowGenerator*/)
{
  return nullptr;
}

bool ShadowDepthWrapper::ShadowDepthWrapperImpl::isReadyForSubMesh(
  SubMesh* /*subMesh*/, const std::vector<std::string>& /*defines*/,
  ShadowGenerator* /*shadowGenerator*/, bool /*useInstances*/)
{
  return false;
}

void ShadowDepthWrapper::ShadowDepthWrapperImpl::dispose()
{
}

Effect*
ShadowDepthWrapper::ShadowDepthWrapperImpl::_makeEffect(SubMesh* /*subMesh*/,
                                                        const std::vector<std::string>& /*defines*/,
                                                        ShadowGenerator* /*shadowGenerator*/)
{
  return nullptr;
}

ShadowDepthWrapper::ShadowDepthWrapper(const MaterialPtr& baseMaterial, Scene* scene,
                                       const std::optional<IIOptionShadowDepthMaterial>& options)
    : _matriceNames{this, &ShadowDepthWrapper::get__matriceNames}
    , standalone{this, &ShadowDepthWrapper::get_standalone}
    , baseMaterial{this, &ShadowDepthWrapper::get_baseMaterial}
    , _impl{
        std::make_unique<ShadowDepthWrapper::ShadowDepthWrapperImpl>(baseMaterial, scene, options)}
{
}

ShadowDepthWrapper::~ShadowDepthWrapper() = default;

std::unordered_map<std::string, std::string>& ShadowDepthWrapper::get__matriceNames()
{
  return _impl->_matriceNames;
}

bool ShadowDepthWrapper::get_standalone() const
{
  return _impl->get_standalone();
}

MaterialPtr& ShadowDepthWrapper::get_baseMaterial()
{
  return _impl->get_baseMaterial();
}

Effect* ShadowDepthWrapper::getEffect(SubMesh* subMesh, ShadowGenerator* shadowGenerator)
{
  return _impl->getEffect(subMesh, shadowGenerator);
}

bool ShadowDepthWrapper::isReadyForSubMesh(SubMesh* subMesh,
                                           const std::vector<std::string>& defines,
                                           ShadowGenerator* shadowGenerator, bool useInstances)
{
  return _impl->isReadyForSubMesh(subMesh, defines, shadowGenerator, useInstances);
}

void ShadowDepthWrapper::dispose()
{
  return _impl->dispose();
}

} // end of namespace BABYLON
