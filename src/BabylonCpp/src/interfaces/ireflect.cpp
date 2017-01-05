#include <babylon/interfaces/ireflect.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

IReflect::IReflect()
{
}

IReflect::~IReflect()
{
}

std::string IReflect::TypeToString(Type type)
{
  switch (type) {
    case Type::NODE:
      return "Node";
    case Type::SCENE:
      return "Scene";
    // Bones
    case Type::BONE:
      return "Bone";
    // Cameras
    case Type::CAMERA:
      return "Camera";
    case Type::ARCFOLLOWCAMERA:
      return "ArcFollowCamera";
    case Type::ARCROTATECAMERA:
      return "ArcRotateCamera";
    case Type::FOLLOWCAMERA:
      return "FollowCamera";
    case Type::FREECAMERA:
      return "FreeCamera";
    case Type::TARGETCAMERA:
      return "TargetCamera";
    // Lights
    case Type::LIGHT:
      return "Light";
    case Type::DIRECTIONALLIGHT:
      return "DirectionalLight";
    case Type::HEMISPHERICLIGHT:
      return "HemisphericLight";
    case Type::POINTLIGHT:
      return "PointLight";
    case Type::SPOTLIGHT:
      return "SpotLight";
    // Materials
    case Type::MATERIAL:
      return "Material";
    case Type::MULTIMATERIAL:
      return "MultiMaterial";
    case Type::SHADERMATERIAL:
      return "ShaderMaterial";
    case Type::STANDARDMATERIAL:
      return "StandardMaterial";
    case Type::PBRMATERIAL:
      return "PBRMaterial";
    // Meshes
    case Type::ABSTRACTMESH:
      return "AbstractMesh";
    case Type::GROUNDMESH:
      return "GroundMesh";
    case Type::INSTANCEDMESH:
      return "InstancedMesh";
    case Type::LINESMESH:
      return "LinesMesh";
    case Type::MESH:
      return "Mesh";
    // Textures
    case Type::BASETEXTURE:
      return "BaseTexture";
    case Type::TEXTURE:
      return "Texture";
    default:
      return "Unknown";
  }
}

any IReflect::getProperty(const std::string& targetProperty)
{
  any property = this;

  return getProperty(property, targetProperty);
}

any IReflect::getProperty(const any& property,
                          const std::string& targetProperty)
{
  any _property = nullptr;
  if (property.is<std::nullptr_t>()) {
    return _property;
  }

  // IAnimatable
  if (property.is<IAnimatable*>()) {
    switch (property._<IAnimatable*>()->type()) {
      // Meshes
      case Type::ABSTRACTMESH:
      case Type::GROUNDMESH:
      case Type::INSTANCEDMESH:
      case Type::LINESMESH:
      case Type::MESH: {
        auto mesh = dynamic_cast<AbstractMesh*>(this);
        if (auto vector3Property = _getVector3Property(mesh, targetProperty)) {
          _property = vector3Property;
        }
        else if (auto quaternionProperty
                 = _getQuaternionProperty(mesh, targetProperty)) {
          _property = quaternionProperty;
        }
      } break;
      default:
        break;
    }
    return _property;
  }

  // Vector 3
  if (property.is<Vector3*>()) {
    if (targetProperty == "x") {
      _property = &property._<Vector3*>()->x;
    }
    else if (targetProperty == "y") {
      _property = &property._<Vector3*>()->y;
    }
    else if (targetProperty == "z") {
      _property = &property._<Vector3*>()->z;
    }
    return _property;
  }

  return _property;
}

void IReflect::setProperty(const any& oldProperty,
                           const std::string& targetProperty,
                           const any& newProperty)
{
  if (oldProperty.is<std::nullptr_t>() || newProperty.is<std::nullptr_t>()) {
    return;
  }

  auto _propertyToUpdate = getProperty(oldProperty, targetProperty);
  if (_propertyToUpdate.is<std::nullptr_t>()) {
    return;
  }

  // Vector3 property update
  if (_propertyToUpdate.is<Vector3*>() && newProperty.is<Vector3 const*>()) {
    *_propertyToUpdate._<Vector3*>() = *newProperty._<Vector3 const*>();
  }
  // Quaternion property update
  if (_propertyToUpdate.is<Quaternion*>()
      && newProperty.is<Quaternion const*>()) {
    *_propertyToUpdate._<Quaternion*>() = *newProperty._<Quaternion const*>();
  }
  // Float property update
  else if (_propertyToUpdate.is<float*>() && newProperty.is<float const*>()) {
    *_propertyToUpdate._<float*>() = *newProperty._<float const*>();
  }
}

Vector3* IReflect::_getVector3Property(AbstractMesh* target,
                                       const std::string& targetProperty)
{
  Vector3* vector3Property = nullptr;
  if (targetProperty == "rotation") {
    vector3Property = &target->rotation();
  }
  else if (targetProperty == "position") {
    vector3Property = &target->position();
  }
  else if (targetProperty == "scaling") {
    vector3Property = &target->scaling();
  }

  return vector3Property;
}

Quaternion* IReflect::_getQuaternionProperty(AbstractMesh* target,
                                             const std::string& targetProperty)
{
  Quaternion* quaternionProperty = nullptr;
  if (targetProperty == "rotationQuaternion") {
    quaternionProperty = &target->rotationQuaternion();
  }
  return quaternionProperty;
}

} // end of namespace BABYLON
