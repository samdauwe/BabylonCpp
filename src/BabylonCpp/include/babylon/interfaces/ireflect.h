#ifndef BABYLON_INTERFACES_IREFLECT_H
#define BABYLON_INTERFACES_IREFLECT_H

#include <babylon/babylon_global.h>
#include <babylon/core/any.h>

namespace BABYLON {

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
    PARTICLE       = 200,
    PARTICLESYSTEM = 201,
    // Texture
    BASETEXTURE = 250,
    TEXTURE     = 251,
    // Morph
    MORPHTARGET = 300
  }; // end of enum class Type

public:
  IReflect();
  virtual ~IReflect();

  virtual Type type() const = 0;
  static string_t TypeToString(Type type);

  any getProperty(const string_t& targetPropertyPath);
  any getProperty(const any& property, const string_t& targetProperty);
  void setProperty(const any& oldProperty, const string_t& targetProperty,
                   const any& property);

private:
  Vector3* _getVector3Property(AbstractMesh* target,
                               const string_t& targetProperty);
  nullable_t<Quaternion> _getQuaternionProperty(AbstractMesh* target,
                                                const string_t& targetProperty);

}; // end of class IReflect

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_IREFLECT_H
