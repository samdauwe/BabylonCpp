#ifndef BABYLON_INTERFACES_IREFLECT_H
#define BABYLON_INTERFACES_IREFLECT_H

#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/core/any.h>

namespace BABYLON {

class AbstractMesh;
class Quaternion;
class Vector3;

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT IReflect {

public:
  enum class Type : unsigned int {
    NODE  = 0,
    SCENE = 1,
    // Bones
    BONE     = 2,
    SKELETON = 3,
    // Cameras
    CAMERA                  = 10,
    ARCFOLLOWCAMERA         = 11,
    ARCROTATECAMERA         = 12,
    DEVICEORIENTATIONCAMERA = 13,
    FOLLOWCAMERA            = 14,
    FREECAMERA              = 15,
    TARGETCAMERA            = 16,
    // Lights
    LIGHT            = 50,
    DIRECTIONALLIGHT = 51,
    HEMISPHERICLIGHT = 52,
    POINTLIGHT       = 53,
    SPOTLIGHT        = 54,
    // Materials
    MATERIAL         = 100,
    MULTIMATERIAL    = 101,
    SHADERMATERIAL   = 102,
    STANDARDMATERIAL = 103,
    PBRMATERIAL      = 104,
    NORMALMATERIAL   = 105,
    PUSHMATERIAL     = 106,
    // Meshes
    ABSTRACTMESH  = 150,
    GROUNDMESH    = 151,
    INSTANCEDMESH = 152,
    LINESMESH     = 153,
    MESH          = 154,
    TRANSFORMNODE = 155,
    // Particles
    PARTICLE          = 200,
    PARTICLESYSTEM    = 201,
    GPUPARTICLESYSTEM = 202,
    // Texture
    BASETEXTURE = 250,
    TEXTURE     = 251,
    // Morph
    MORPHTARGET = 300,
    // Piplines
    STANDARDRENDERINGPIPLINE = 350
  }; // end of enum class Type

public:
  IReflect();
  virtual ~IReflect();

  virtual Type type() const = 0;
  static std::string TypeToString(Type type);

  any getProperty(const std::string& targetPropertyPath);
  any getProperty(const any& property, const std::string& targetProperty);
  void setProperty(const any& oldProperty, const std::string& targetProperty,
                   const any& property);

private:
  Vector3* _getVector3Property(AbstractMesh* target,
                               const std::string& targetProperty);
  std::optional<Quaternion>
  _getQuaternionProperty(AbstractMesh* target,
                         const std::string& targetProperty);

}; // end of class IReflect

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_IREFLECT_H
