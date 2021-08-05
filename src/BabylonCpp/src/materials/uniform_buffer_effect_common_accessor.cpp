#include <babylon/materials/uniform_buffer_effect_common_accessor.h>

#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>

namespace BABYLON {

UniformBufferEffectCommonAccessor::UniformBufferEffectCommonAccessor(
  const std::variant<UniformBuffer*, Effect*>& uboOrEffect)
{
  if (_isUbo(uboOrEffect)) {
    const auto& ubo = std::get<UniformBuffer*>(uboOrEffect);
    setMatrix3x3    = [ubo](const std::string& name, const Float32Array& matrix) -> void {
      ubo->updateMatrix3x3(name, matrix); //
    };
    setMatrix2x2 = [ubo](const std::string& name, const Float32Array& matrix) -> void {
      ubo->updateMatrix2x2(name, matrix); //
    };
    setFloat = [ubo](const std::string& name, float x) -> void {
      ubo->updateFloat(name, x); //
    };
    setFloat2
      = [ubo](const std::string& name, float x, float y, const std::string& suffix) -> void {
      ubo->updateFloat2(name, x, y, suffix); //
    };
    setFloat3 = [ubo](const std::string& name, float x, float y, float z,
                      const std::string& suffix) -> void {
      ubo->updateFloat3(name, x, y, z, suffix); //
    };
    setFloat4 = [ubo](const std::string& name, float x, float y, float z, float w,
                      const std::string& suffix) -> void {
      ubo->updateFloat4(name, x, y, z, w, suffix); //
    };
    setFloatArray = [ubo](const std::string& name, const Float32Array& array) -> void {
      ubo->updateFloatArray(name, array); //
    };
    setArray = [ubo](const std::string& name, const Float32Array& array) -> void {
      ubo->updateArray(name, array); //
    };
    setIntArray = [ubo](const std::string& name, const Int32Array& array) -> void {
      ubo->updateIntArray(name, array); //
    };
    setMatrix = [ubo](const std::string& name, const Matrix& mat) -> void {
      ubo->updateMatrix(name, mat); //
    };
    setMatrices = [ubo](const std::string& name, const Float32Array& mat) -> void {
      ubo->updateMatrices(name, mat); //
    };
    setVector3 = [ubo](const std::string& name, const Vector3& vector) -> void {
      ubo->updateVector3(name, vector); //
    };
    setVector4 = [ubo](const std::string& name, const Vector4& vector) -> void {
      ubo->updateVector4(name, vector); //
    };
    setColor3
      = [ubo](const std::string& name, const Color3& color, const std::string& suffix) -> void {
      ubo->updateColor3(name, color, suffix); //
    };
    setColor4 = [ubo](const std::string& name, const Color3& color, float alpha,
                      const std::string& suffix) -> void {
      ubo->updateColor4(name, color, alpha, suffix); //
    };
    setDirectColor4 = [ubo](const std::string& name, const Color4& color) -> void {
      ubo->updateDirectColor4(name, color, ""); //
    };
    setInt = [ubo](const std::string& name, int32_t x, const std::string& suffix) -> void {
      ubo->updateInt(name, x, suffix); //
    };
    setInt2
      = [ubo](const std::string& name, int32_t x, int32_t y, const std::string& suffix) -> void {
      ubo->updateInt2(name, x, y, suffix); //
    };
    setInt3 = [ubo](const std::string& name, int32_t x, int32_t y, int32_t z,
                    const std::string& suffix) -> void {
      ubo->updateInt3(name, x, y, z, suffix); //
    };
    setInt4 = [ubo](const std::string& name, int32_t x, int32_t y, int32_t z, int32_t w,
                    const std::string& suffix) -> void {
      ubo->updateInt4(name, x, y, z, w, suffix); //
    };
  }
  else {
    const auto& effect = std::get<Effect*>(uboOrEffect);
    setMatrix3x3       = [effect](const std::string& name, const Float32Array& matrix) -> void {
      effect->setMatrix3x3(name, matrix); //
    };
    setMatrix2x2 = [effect](const std::string& name, const Float32Array& matrix) -> void {
      effect->setMatrix2x2(name, matrix); //
    };
    setFloat = [effect](const std::string& name, float x) -> void {
      effect->setFloat(name, x); //
    };
    setFloat2
      = [effect](const std::string& name, float x, float y, const std::string& /*suffix*/) -> void {
      effect->setFloat2(name, x, y); //
    };
    setFloat3 = [effect](const std::string& name, float x, float y, float z,
                         const std::string& /*suffix*/) -> void {
      effect->setFloat3(name, x, y, z); //
    };
    setFloat4 = [effect](const std::string& name, float x, float y, float z, float w,
                         const std::string& /*suffix*/) -> void {
      effect->setFloat4(name, x, y, z, w); //
    };
    setFloatArray = [effect](const std::string& name, const Float32Array& array) -> void {
      effect->setFloatArray(name, array); //
    };
    setArray = [effect](const std::string& name, const Float32Array& array) -> void {
      effect->setArray(name, array); //
    };
    setIntArray = [effect](const std::string& name, const Int32Array& array) -> void {
      effect->setIntArray(name, array); //
    };
    setMatrix = [effect](const std::string& name, const Matrix& mat) -> void {
      effect->setMatrix(name, mat); //
    };
    setMatrices = [effect](const std::string& name, const Float32Array& mat) -> void {
      effect->setMatrices(name, mat); //
    };
    setVector3 = [effect](const std::string& name, const Vector3& vector) -> void {
      effect->setVector3(name, vector); //
    };
    setVector4 = [effect](const std::string& name, const Vector4& vector) -> void {
      effect->setVector4(name, vector); //
    };
    setColor3 = [effect](const std::string& name, const Color3& color,
                         const std::string& /*suffix*/) -> void {
      effect->setColor3(name, color); //
    };
    setColor4 = [effect](const std::string& name, const Color3& color, float alpha,
                         const std::string& /*suffix*/) -> void {
      effect->setColor4(name, color, alpha); //
    };
    setDirectColor4 = [effect](const std::string& name, const Color4& color) -> void {
      effect->setDirectColor4(name, color); //
    };
    setInt = [effect](const std::string& name, int32_t x, const std::string& /*suffix*/) -> void {
      effect->setInt(name, x); //
    };
    setInt2 = [effect](const std::string& name, int32_t x, int32_t y,
                       const std::string& /*suffix*/) -> void {
      effect->setInt2(name, x, y); //
    };
    setInt3 = [effect](const std::string& name, int32_t x, int32_t y, int32_t z,
                       const std::string& /*suffix*/) -> void {
      effect->setInt3(name, x, y, z); //
    };
    setInt4 = [effect](const std::string& name, int32_t x, int32_t y, int32_t z, int32_t w,
                       const std::string& /*suffix*/) -> void {
      effect->setInt4(name, x, y, z, w); //
    };
  }
}

UniformBufferEffectCommonAccessor::~UniformBufferEffectCommonAccessor() = default;

bool UniformBufferEffectCommonAccessor::_isUbo(
  const std::variant<UniformBuffer*, Effect*>& uboOrEffect) const
{
  return std::holds_alternative<UniformBuffer*>(uboOrEffect);
}

} // end of namespace BABYLON
