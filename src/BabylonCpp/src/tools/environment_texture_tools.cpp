#include <babylon/tools/environment_texture_tools.h>

#include <sstream>

#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/math/spherical_polynomial.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/environment_texture_info.h>

namespace BABYLON {

std::array<uint8_t, 8> EnvironmentTextureTools::_MagicBytes
  = {0x86, 0x16, 0x87, 0x96, 0xf6, 0xd6, 0x96, 0x36};

EnvironmentTextureTools::EnvironmentTextureTools()
{
}

EnvironmentTextureTools::~EnvironmentTextureTools()
{
}

std::optional<EnvironmentTextureInfo>
EnvironmentTextureTools::GetEnvInfo(const ArrayBuffer& data)
{
  const auto& dataView = data;
  size_t pos           = 0;

  for (unsigned int i = 0; i < EnvironmentTextureTools::_MagicBytes.size();
       ++i) {
    if (dataView[pos++] != EnvironmentTextureTools::_MagicBytes[i]) {
      BABYLON_LOG_ERROR("EnvironmentTextureTools",
                        "Not a babylon environment map");
      return std::nullopt;
    }
  }

  // Read json manifest - collect characters up to null terminator
  std::ostringstream manifestString;
  char charCode = 0x00;
  while ((charCode = static_cast<char>(dataView[pos++]))) {
    manifestString << String::fromCharCode(charCode);
  }

  // Parse JSON string
  auto parsedManifest = json::parse(manifestString.str());

  EnvironmentTextureInfo manifest;
  manifest.version = json_util::get_number(parsedManifest, "version", 0u);
  manifest.width   = json_util::get_number(parsedManifest, "width", 0);

  // Irradiance information
  if (json_util::has_key(parsedManifest, "irradiance")) {
    auto parsedIrradiance = parsedManifest["irradiance"];

    EnvironmentTextureIrradianceInfoV1 irradiance;
    irradiance.x = json_util::get_array<float>(parsedIrradiance, "x");
    irradiance.y = json_util::get_array<float>(parsedIrradiance, "y");
    irradiance.z = json_util::get_array<float>(parsedIrradiance, "z");

    irradiance.xx = json_util::get_array<float>(parsedIrradiance, "xx");
    irradiance.yy = json_util::get_array<float>(parsedIrradiance, "yy");
    irradiance.zz = json_util::get_array<float>(parsedIrradiance, "zz");

    irradiance.yz = json_util::get_array<float>(parsedIrradiance, "yz");
    irradiance.zx = json_util::get_array<float>(parsedIrradiance, "zx");
    irradiance.xy = json_util::get_array<float>(parsedIrradiance, "xy");

    manifest.irradiance = irradiance;
  }

  // Specular information
  if (json_util::has_key(parsedManifest, "specular")) {
    EnvironmentTextureSpecularInfoV1 specular;
    manifest.specular = specular;
  }

  if (manifest.specular.has_value()) {
    // Extend the header with the position of the payload.
    manifest.specular->specularDataPosition = pos;
    // Fallback to 0.8 exactly if lodGenerationScale is not defined for backward
    // compatibility.
    manifest.specular->lodGenerationScale
      = manifest.specular->lodGenerationScale.has_value() ?
          *manifest.specular->lodGenerationScale :
          0.8f;
  }

  return manifest;
}

bool EnvironmentTextureTools::UploadEnvLevels(
  const InternalTexturePtr& /*texture*/, const ArrayBuffer& /*arrayBuffer*/,
  const EnvironmentTextureInfo& /*info*/)
{
  return false;
}

void EnvironmentTextureTools::UploadEnvSpherical(
  const InternalTexturePtr& texture, const EnvironmentTextureInfo& info)
{
  if (info.version != 1) {
    BABYLON_LOGF_WARN("EnvironmentTextureTools",
                      "Unsupported babylon environment map version \"%u\"",
                      info.version);
  }

  auto irradianceInfo = info.irradiance;
  if (!irradianceInfo) {
    return;
  }

  auto sp = std::make_shared<SphericalPolynomial>();
  Vector3::FromArrayToRef(irradianceInfo->x, 0, sp->x);
  Vector3::FromArrayToRef(irradianceInfo->y, 0, sp->y);
  Vector3::FromArrayToRef(irradianceInfo->z, 0, sp->z);
  Vector3::FromArrayToRef(irradianceInfo->xx, 0, sp->xx);
  Vector3::FromArrayToRef(irradianceInfo->yy, 0, sp->yy);
  Vector3::FromArrayToRef(irradianceInfo->zz, 0, sp->zz);
  Vector3::FromArrayToRef(irradianceInfo->yz, 0, sp->yz);
  Vector3::FromArrayToRef(irradianceInfo->zx, 0, sp->zx);
  Vector3::FromArrayToRef(irradianceInfo->xy, 0, sp->xy);
  texture->_sphericalPolynomial = sp;
}

} // end of namespace BABYLON
