#ifndef BABYLON_MESH_BASE_SUB_MESH_H
#define BABYLON_MESH_BASE_SUB_MESH_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Effect;
struct MaterialDefines;

class BABYLON_SHARED_EXPORT BaseSubMesh {

public:
  BaseSubMesh();
  ~BaseSubMesh();

  Effect* effect();
  void setEffect(Effect* effect);
  void setEffect(Effect* effect, const MaterialDefines& defines);

public:
  /** Hidden */
  std::unique_ptr<MaterialDefines> _materialDefines;
  /** Hidden */
  Effect* _materialEffect;

}; // end of class BaseSubMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_BASE_SUB_MESH_H
