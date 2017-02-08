#include <babylon/loading/plugins/gltf/gltf_file_loader_utils.h>

#include <babylon/engine/node.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/matrix.h>

namespace BABYLON {

void GLTFUtils::SetMatrix(Scene* scene, Node* source,
                          const IGLTFTechniqueParameter& parameter,
                          const std::string& uniformName,
                          ShaderMaterial* shaderMaterial)
{
  Matrix mat;

  if (parameter.semantic == "MODEL") {
    mat = *source->getWorldMatrix();
  }
  else if (parameter.semantic == "PROJECTION") {
    mat = scene->getProjectionMatrix();
  }
  else if (parameter.semantic == "VIEW") {
    mat = scene->getViewMatrix();
  }
  else if (parameter.semantic == "MODELVIEWINVERSETRANSPOSE") {
    mat = Matrix::Transpose(
      source->getWorldMatrix()->multiply(scene->getViewMatrix()).invert());
  }
  else if (parameter.semantic == "MODELVIEW") {
    mat = source->getWorldMatrix()->multiply(scene->getViewMatrix());
  }
  else if (parameter.semantic == "MODELVIEWPROJECTION") {
    mat = source->getWorldMatrix()->multiply(scene->getTransformMatrix());
  }
  else if (parameter.semantic == "MODELINVERSE") {
    mat = source->getWorldMatrix()->invert();
  }
  else if (parameter.semantic == "VIEWINVERSE") {
    mat = scene->getViewMatrix().invert();
  }
  else if (parameter.semantic == "PROJECTIONINVERSE") {
    mat = scene->getProjectionMatrix().invert();
  }
  else if (parameter.semantic == "MODELVIEWINVERSE") {
    mat = source->getWorldMatrix()->multiply(scene->getViewMatrix()).invert();
  }
  else if (parameter.semantic == "MODELVIEWPROJECTIONINVERSE") {
    mat = source->getWorldMatrix()
            ->multiply(scene->getTransformMatrix())
            .invert();
  }
  else if (parameter.semantic == "MODELINVERSETRANSPOSE") {
    mat = Matrix::Transpose(source->getWorldMatrix()->invert());
  }

  switch (parameter.type) {
    case EParameterType::FLOAT_MAT2:
      shaderMaterial->setMatrix2x2(uniformName, Matrix::GetAsMatrix2x2(mat));
      break;
    case EParameterType::FLOAT_MAT3:
      shaderMaterial->setMatrix3x3(uniformName, Matrix::GetAsMatrix3x3(mat));
      break;
    case EParameterType::FLOAT_MAT4:
      shaderMaterial->setMatrix(uniformName, mat);
      break;
    default:
      break;
  }
}

bool GLTFUtils::SetUniform(ShaderMaterial* shaderMaterial,
                           const std::string& uniform, any value,
                           EParameterType type)
{
  switch (type) {
    case EParameterType::FLOAT:
      shaderMaterial->setFloat(uniform, value._<float>());
      return true;
    case EParameterType::FLOAT_VEC2:
      shaderMaterial->setVector2(uniform,
                                 Vector2::FromArray(value._<Float32Array>()));
      return true;
    case EParameterType::FLOAT_VEC3:
      shaderMaterial->setVector3(uniform,
                                 Vector3::FromArray(value._<Float32Array>()));
      return true;
    case EParameterType::FLOAT_VEC4:
      shaderMaterial->setVector4(uniform,
                                 Vector4::FromArray(value._<Float32Array>()));
      return true;
    default:
      return false;
  }
}

bool GLTFUtils::IsBase64(const std::string& uri)
{
  return uri.size() < 5 ? false : uri.substr(0, 5) == "data:";
}

unsigned int GLTFUtils::GetWrapMode(ETextureWrapMode mode)
{
  switch (mode) {
    case ETextureWrapMode::CLAMP_TO_EDGE:
      return Texture::CLAMP_ADDRESSMODE;
    case ETextureWrapMode::MIRRORED_REPEAT:
      return Texture::MIRROR_ADDRESSMODE;
    case ETextureWrapMode::REPEAT:
      return Texture::WRAP_ADDRESSMODE;
    default:
      return Texture::WRAP_ADDRESSMODE;
  }
}

unsigned int GLTFUtils::GetByteStrideFromType(const IGLTFAccessor& accessor)
{
  // Needs this function since "byteStride" isn't requiered in glTF format
  const auto& type = accessor.type;

  if (type == "VEC2") {
    return 2;
  }
  else if (type == "VEC3") {
    return 3;
  }
  else if (type == "VEC4") {
    return 4;
  }
  else if (type == "MAT2") {
    return 4;
  }
  else if (type == "MAT3") {
    return 9;
  }
  else if (type == "MAT4") {
    return 16;
  }
  else {
    return 1;
  }
}

unsigned int GLTFUtils::GetTextureFilterMode(ETextureFilterType mode)
{
  switch (mode) {
    case ETextureFilterType::LINEAR:
    case ETextureFilterType::LINEAR_MIPMAP_NEAREST:
    case ETextureFilterType::LINEAR_MIPMAP_LINEAR:
      return Texture::TRILINEAR_SAMPLINGMODE;
    case ETextureFilterType::NEAREST_MIPMAP_NEAREST:
      return Texture::NEAREST_SAMPLINGMODE;
    default:
      return Texture::BILINEAR_SAMPLINGMODE;
  }
}

} // end of namespace BABYLON
