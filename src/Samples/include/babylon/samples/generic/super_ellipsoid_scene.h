#ifndef BABYLON_SAMPLES_GENERIC_SUPER_ELLIPSOID_H
#define BABYLON_SAMPLES_GENERIC_SUPER_ELLIPSOID_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class SuperEllipsoidScene : public IRenderableScene {

public:
  SuperEllipsoidScene(ICanvas* iCanvas);
  ~SuperEllipsoidScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  Vector3 _sampleSuperEllipsoid(float phi, float beta, float n1, float n2,
                                float scaleX, float scaleY, float scaleZ) const;
  Vector3 _calculateNormal(float phi, float beta, float n1, float n2,
                           float scaleX, float scaleY, float scaleZ) const;
  Mesh* _createSuperEllipsoid(size_t samples, float n1, float n2, float scalex,
                              float scaley, float scalez, Scene* scene);

}; // end of class SuperEllipsoidScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_GENERIC_SUPER_ELLIPSOID_H
