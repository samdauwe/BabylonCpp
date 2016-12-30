#ifndef BABYLON_MATERIALS_IMATERIAL_DEFINES_H
#define BABYLON_MATERIALS_IMATERIAL_DEFINES_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IMaterialDefines {
  virtual bool isEqual(MaterialDefines& other) const = 0;
  virtual void cloneTo(MaterialDefines& other)       = 0;
  virtual void reset()                               = 0;
  virtual std::string toString() const               = 0;
}; // end of struct IMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_IMATERIALS_MATERIAL_DEFINES_H