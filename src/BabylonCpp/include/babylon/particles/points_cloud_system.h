#ifndef BABYLON_PARTICLES_POINTS_CLOUD_SYSTEM_H
#define BABYLON_PARTICLES_POINTS_CLOUD_SYSTEM_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

class BABYLON_SHARED_EXPORT PointsCloudSystem {

public:
  MeshPtr mesh;

}; // end of class PointsCloudSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_POINTS_CLOUD_SYSTEM_H
