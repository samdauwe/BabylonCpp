#ifndef BABYLON_MESHES_INTERNAL_ABSTRACT_MESH_DATA_INFO_H
#define BABYLON_MESHES_INTERNAL_ABSTRACT_MESH_DATA_INFO_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/meshes/_facet_data_storage.h>

namespace BABYLON {

class Skeleton;
using SkeletonPtr = std::shared_ptr<Skeleton>;

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _InternalAbstractMeshDataInfo {
  bool _hasVertexAlpha             = false;
  bool _useVertexColors            = true;
  unsigned int _numBoneInfluencers = 4;
  bool _applyFog                   = true;
  bool _receiveShadows             = false;
  _FacetDataStorage _facetData;
  float _visibility                  = 1.f;
  SkeletonPtr _skeleton              = nullptr;
  unsigned int _layerMask            = 0x0FFFFFFF;
  bool _computeBonesUsingShaders     = true;
  bool _isActive                     = false;
  bool _onlyForInstances             = false;
  bool _isActiveIntermediate         = false;
  bool _onlyForInstancesIntermediate = false;
  bool _actAsRegularMesh             = false;
}; // end of struct _InternalAbstractMeshDataInfo

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_INTERNAL_ABSTRACT_MESH_DATA_INFO_H
