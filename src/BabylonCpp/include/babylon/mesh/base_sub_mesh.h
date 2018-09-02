#ifndef BABYLON_MESH_BASE_SUB_MESH_H
#define BABYLON_MESH_BASE_SUB_MESH_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BaseSubMesh {

public:
  BaseSubMesh();
  ~BaseSubMesh();

  Effect* effect();
  void setEffect(Effect* effect);
  void setEffect(Effect* effect, const MaterialDefines& defines);

public:
  /** Hidden */
  unique_ptr_t<MaterialDefines> _materialDefines;
  /** Hidden */
  Effect* _materialEffect;

}; // end of class BaseSubMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_BASE_SUB_MESH_H
