#include <babylon/interfaces/ireflect.h>

#include <babylon/materials/material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

IReflect::IReflect()
{
}

IReflect::~IReflect()
{
}

string_t IReflect::TypeToString(Type type)
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
    case Type::NORMALMATERIAL:
      return "NormalMaterial";
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

any IReflect::getProperty(const string_t& targetProperty)
{
  any property = dynamic_cast<IReflect*>(this);

  return getProperty(property, targetProperty);
}

any IReflect::getProperty(const any& property, const string_t& targetProperty)
{
  any _property = nullptr;
  if (property.is<std::nullptr_t>()) {
    return _property;
  }

  // IAnimatable
  if (property.is<IReflect*>() || property.is<IAnimatable*>()) {
    switch (property._<IReflect*>()->type()) {
      // Meshes
      case Type::ABSTRACTMESH:
      case Type::GROUNDMESH:
      case Type::INSTANCEDMESH:
      case Type::LINESMESH:
      case Type::MESH: {
        auto mesh = dynamic_cast<AbstractMesh*>(this);
        // Vector3 property
        if (auto vector3Property = _getVector3Property(mesh, targetProperty)) {
          _property = vector3Property;
        }
        // Quaternion property
        else if (auto quaternionProperty
                 = _getQuaternionProperty(mesh, targetProperty)) {
          _property = quaternionProperty;
        }
        // Material Property
        else if (targetProperty == "material") {
          _property = mesh->material();
        }
      } break;
      default:
        break;
    }
    return _property;
  }

  // Float
  if (property.is<float*>()) {
    _property = &property._<float*>();
    return _property;
  }

  // Quaternion
  if (property.is<Quaternion*>()) {
    if (targetProperty == "x") {
      _property = &property._<Quaternion*>()->x;
    }
    else if (targetProperty == "y") {
      _property = &property._<Quaternion*>()->y;
    }
    else if (targetProperty == "z") {
      _property = &property._<Quaternion*>()->z;
    }
    else if (targetProperty == "w") {
      _property = &property._<Quaternion*>()->w;
    }
    return _property;
  }

  // Vector3
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

  // Vector2
  if (property.is<Vector2*>()) {
    if (targetProperty == "x") {
      _property = &property._<Vector2*>()->x;
    }
    else if (targetProperty == "y") {
      _property = &property._<Vector2*>()->y;
    }
    return _property;
  }

  // Size
  if (property.is<size_t*>()) {
    _property = &property._<size_t*>();
    return _property;
  }

  // Color3
  if (property.is<Color3*>()) {
    if (targetProperty == "x") {
      _property = &property._<Color3*>()->r;
    }
    else if (targetProperty == "y") {
      _property = &property._<Color3*>()->g;
    }
    else if (targetProperty == "z") {
      _property = &property._<Color3*>()->b;
    }
    return _property;
  }

  // Material
  if (property.is<Material*>()) {
    // Standard material
    if (property._<StandardMaterial*>()) {
      if (targetProperty == "ambientColor") {
        _property = &property._<StandardMaterial*>()->ambientColor;
      }
      else if (targetProperty == "diffuseColor") {
        _property = &property._<StandardMaterial*>()->diffuseColor;
      }
      else if (targetProperty == "specularColor") {
        _property = &property._<StandardMaterial*>()->specularColor;
      }
      else if (targetProperty == "emissiveColor") {
        _property = &property._<StandardMaterial*>()->emissiveColor;
      }
    }
  }

  return _property;
}

void IReflect::setProperty(const any& oldProperty,
                           const string_t& targetProperty,
                           const any& newProperty)
{
  if (oldProperty.is<std::nullptr_t>() || newProperty.is<std::nullptr_t>()) {
    return;
  }

  auto _propertyToUpdate = getProperty(oldProperty, targetProperty);
  if (_propertyToUpdate.is<std::nullptr_t>()) {
    return;
  }

  // Float property update
  if (_propertyToUpdate.is<float*>() && newProperty.is<float const*>()) {
    *_propertyToUpdate._<float*>() = *newProperty._<float const*>();
  }
  // Quaternion property update
  else if (_propertyToUpdate.is<Quaternion*>()
           && newProperty.is<Quaternion const*>()) {
    *_propertyToUpdate._<Quaternion*>() = *newProperty._<Quaternion const*>();
  }
  // Vector3 property update
  else if (_propertyToUpdate.is<Vector3*>()
           && newProperty.is<Vector3 const*>()) {
    *_propertyToUpdate._<Vector3*>() = *newProperty._<Vector3 const*>();
  }
  // Vector2 property update
  else if (_propertyToUpdate.is<Vector2*>()
           && newProperty.is<Vector2 const*>()) {
    *_propertyToUpdate._<Vector2*>() = *newProperty._<Vector2 const*>();
  }
  // Size property update
  else if (_propertyToUpdate.is<size_t*>() && newProperty.is<size_t const*>()) {
    *_propertyToUpdate._<size_t*>() = *newProperty._<size_t const*>();
  }
  // Color3 property update
  else if (_propertyToUpdate.is<Color3*>() && newProperty.is<Color3 const*>()) {
    *_propertyToUpdate._<Color3*>() = *newProperty._<Color3 const*>();
  }
}

Vector3* IReflect::_getVector3Property(AbstractMesh* target,
                                       const string_t& targetProperty)
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
                                             const string_t& targetProperty)
{
  Quaternion* quaternionProperty = nullptr;
  if (targetProperty == "rotationQuaternion") {
    quaternionProperty = &target->rotationQuaternion();
  }
  return quaternionProperty;
}

} // end of namespace BABYLON
