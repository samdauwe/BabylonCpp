#ifndef BABYLON_MATERIALS_UNIFORM_BUFFER_EFFECT_COMMON_ACCESSOR_H
#define BABYLON_MATERIALS_UNIFORM_BUFFER_EFFECT_COMMON_ACCESSOR_H

#include <functional>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Color3;
class Color4;
class Effect;
class Matrix;
class UniformBuffer;
class Vector3;
class Vector4;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT UniformBufferEffectCommonAccessor {

public:
  UniformBufferEffectCommonAccessor(const std::variant<UniformBuffer*, Effect*>& uboOrEffect);
  ~UniformBufferEffectCommonAccessor(); // = default;

private:
  bool _isUbo(const std::variant<UniformBuffer*, Effect*>& uboOrEffect) const;

public:
  std::function<void(const std::string& name, const Float32Array& matrix)> setMatrix3x3;

  std::function<void(const std::string& name, const Float32Array& matrix)> setMatrix2x2;

  std::function<void(const std::string& name, float x)> setFloat;

  std::function<void(const std::string& name, float x, float y, const std::string& suffix)>
    setFloat2;

  std::function<void(const std::string& name, float x, float y, float z, const std::string& suffix)>
    setFloat3;

  std::function<void(const std::string& name, float x, float y, float z, float w,
                     const std::string& suffix)>
    setFloat4;

  std::function<void(const std::string& name, const Float32Array& array)> setFloatArray;

  std::function<void(const std::string& name, const Float32Array& array)> setArray;

  std::function<void(const std::string& name, const Int32Array& array)> setIntArray;

  std::function<void(const std::string& name, const Matrix& mat)> setMatrix;

  std::function<void(const std::string& name, const Float32Array& mat)> setMatrices;

  std::function<void(const std::string& name, const Vector3& vector)> setVector3;

  std::function<void(const std::string& name, const Vector4& vector)> setVector4;

  std::function<void(const std::string& name, const Color3& color, const std::string& suffix)>
    setColor3;

  std::function<void(const std::string& name, const Color3& color, float alpha,
                     const std::string& suffix)>
    setColor4;

  std::function<void(const std::string& name, const Color4& color)> setDirectColor4;

  std::function<void(const std::string& name, int32_t x, const std::string& suffix)> setInt;

  std::function<void(const std::string& name, int32_t x, int32_t y, const std::string& suffix)>
    setInt2;

  std::function<void(const std::string& name, int32_t x, int32_t y, int32_t z,
                     const std::string& suffix)>
    setInt3;

  std::function<void(const std::string& name, int32_t x, int32_t y, int32_t z, int32_t w,
                     const std::string& suffix)>
    setInt4;

}; // end of class UniformBufferEffectCommonAccessor

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_UNIFORM_BUFFER_EFFECT_COMMON_ACCESSOR_H
