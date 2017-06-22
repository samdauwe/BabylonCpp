#ifndef BABYLON_ISHADOW_LIGHT_H
#define BABYLON_ISHADOW_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/lights/light.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT IShadowLight : public Light {

public:
  IShadowLight(const std::string& name, Scene* scene);
  virtual ~IShadowLight();

  virtual bool computeTransformedInformation() = 0;
  virtual Scene* getScene()                    = 0;

  virtual IShadowLight*
  setShadowProjectionMatrix(Matrix& matrix, Matrix& viewMatrix,
                            const std::vector<AbstractMesh*>& renderList)
    = 0;
  virtual float getDepthScale() const = 0;

  virtual bool needCube() const               = 0;
  virtual bool needProjectionMatrixCompute()  = 0;
  virtual void forceProjectionMatrixCompute() = 0;

  virtual Vector3 getShadowDirection(unsigned int faceIndex = 0) = 0;

public:
  Vector3 position;

}; // end of class IShadowLight

} // end of namespace BABYLON

#endif // end of BABYLON_ISHADOW_LIGHT_H
