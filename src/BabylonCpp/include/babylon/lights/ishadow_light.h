#ifndef BABYLON_ISHADOW_LIGHT_H
#define BABYLON_ISHADOW_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/lights/light.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT IShadowLight : public Light {

public:
  IShadowLight(const string_t& name, Scene* scene);
  virtual ~IShadowLight();

  virtual Vector3& direction()            = 0;
  virtual Vector3& transformedPosition()  = 0;
  virtual Vector3& transformedDirection() = 0;

  virtual bool computeTransformedInformation() = 0;
  virtual Scene* getScene()                    = 0;

  virtual IShadowLight*
  setShadowProjectionMatrix(Matrix& matrix, Matrix& viewMatrix,
                            const vector_t<AbstractMesh*>& renderList)
    = 0;
  virtual float getDepthScale() const = 0;

  virtual bool needCube() const               = 0;
  virtual bool needProjectionMatrixCompute()  = 0;
  virtual void forceProjectionMatrixCompute() = 0;

  virtual Vector3 getShadowDirection(unsigned int faceIndex = 0) = 0;

  /**
   * @brief Gets the minZ used for shadow according to both the scene and the
   * light.
   * @param activeCamera
   */
  virtual float getDepthMinZ(Camera* activeCamera) const = 0;

  /**
   * @brief Gets the minZ used for shadow according to both the scene and the
   * light.
   * @param activeCamera
   */
  virtual float getDepthMaxZ(Camera* activeCamera) const = 0;

public:
  Vector3 position;

}; // end of class IShadowLight

} // end of namespace BABYLON

#endif // end of BABYLON_ISHADOW_LIGHT_H
