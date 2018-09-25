#include <babylon/tools/hdr/cube_map_to_spherical_polynomial_tools.h>

#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/math/color3.h>
#include <babylon/math/scalar.h>
#include <babylon/math/spherical_harmonics.h>
#include <babylon/math/spherical_polynomial.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

std::array<FileFaceOrientation, 6> CubeMapToSphericalPolynomialTools::FileFaces
  = {{
    FileFaceOrientation("right", Vector3(1, 0, 0), Vector3(0, 0, -1),
                        Vector3(0, -1, 0)), // +X east
    FileFaceOrientation("left", Vector3(-1, 0, 0), Vector3(0, 0, 1),
                        Vector3(0, -1, 0)), // -X west
    FileFaceOrientation("up", Vector3(0, 1, 0), Vector3(1, 0, 0),
                        Vector3(0, 0, 1)), // +Y north
    FileFaceOrientation("down", Vector3(0, -1, 0), Vector3(1, 0, 0),
                        Vector3(0, 0, -1)), // -Y south
    FileFaceOrientation("front", Vector3(0, 0, 1), Vector3(1, 0, 0),
                        Vector3(0, -1, 0)), // +Z top
    FileFaceOrientation("back", Vector3(0, 0, -1), Vector3(-1, 0, 0),
                        Vector3(0, -1, 0)) // -Z bottom
  }};

std::unique_ptr<SphericalPolynomial>
CubeMapToSphericalPolynomialTools::ConvertCubeMapTextureToSphericalPolynomial(
  BaseTexture* texture)
{
  if (!texture->isCube) {
    // Only supports cube Textures currently.
    return nullptr;
  }

  auto size  = static_cast<std::size_t>(texture->getSize().width);
  auto right = texture->readPixels(0);
  auto left  = texture->readPixels(1);

  ArrayBufferView up;
  ArrayBufferView down;
  if (texture->isRenderTarget) {
    up   = texture->readPixels(3);
    down = texture->readPixels(2);
  }
  else {
    up   = texture->readPixels(2);
    down = texture->readPixels(3);
  }

  auto front = texture->readPixels(4);
  auto back  = texture->readPixels(5);

  auto gammaSpace = texture->gammaSpace;
  // Always read as RGBA.
  auto format = EngineConstants::TEXTUREFORMAT_RGBA;
  auto type   = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
  if (texture->textureType() != EngineConstants::TEXTURETYPE_UNSIGNED_INT) {
    type = EngineConstants::TEXTURETYPE_FLOAT;
  }

  CubeMapInfo cubeInfo{
    front,      //
    back,       //
    left,       //
    right,      //
    up,         //
    down,       //
    size,       //
    format,     //
    type,       //
    gammaSpace, //
  };

  return ConvertCubeMapToSphericalPolynomial(cubeInfo);
}

std::unique_ptr<SphericalPolynomial>
CubeMapToSphericalPolynomialTools::ConvertCubeMapToSphericalPolynomial(
  const CubeMapInfo& cubeInfo)
{
  SphericalHarmonics sphericalHarmonics;
  float totalSolidAngle = 0.f;

  // The (u,v) range is [-1,+1], so the distance between each texel is 2/Size.
  float du = 2.f / static_cast<float>(cubeInfo.size);
  float dv = du;

  // The (u,v) of the first texel is half a texel from the corner (-1,-1).
  float minUV = du * 0.5f - 1.f;

  for (unsigned int faceIndex = 0; faceIndex < 6; ++faceIndex) {
    const auto& fileFace = FileFaces[faceIndex];
    Float32Array dataArray;
    if (fileFace.name == "right") {
      dataArray = cubeInfo.right.float32Array;
    }
    else if (fileFace.name == "left") {
      dataArray = cubeInfo.left.float32Array;
    }
    else if (fileFace.name == "up") {
      dataArray = cubeInfo.up.float32Array;
    }
    else if (fileFace.name == "down") {
      dataArray = cubeInfo.down.float32Array;
    }
    else if (fileFace.name == "front") {
      dataArray = cubeInfo.front.float32Array;
    }
    else if (fileFace.name == "back") {
      dataArray = cubeInfo.back.float32Array;
    }
    float v = minUV;

    // TODO: we could perform the summation directly into a SphericalPolynomial
    // (SP), which is more efficient than SphericalHarmonic (SH).
    // This is possible because during the summation we do not need the
    // SH-specific properties, e.g. orthogonality.
    // Because SP is still linear, so summation is fine in that basis.
    const unsigned int stride
      = cubeInfo.format == EngineConstants::TEXTUREFORMAT_RGBA ? 4 : 3;
    for (size_t y = 0; y < cubeInfo.size; ++y) {
      float u = minUV;

      for (size_t x = 0; x < cubeInfo.size; ++x) {
        // World direction (not normalised)
        auto worldDirection = fileFace.worldAxisForFileX.scale(u)
                                .add(fileFace.worldAxisForFileY.scale(v))
                                .add(fileFace.worldAxisForNormal);
        worldDirection.normalize();

        float deltaSolidAngle = std::pow(1.f + u * u + v * v, -3.f / 2.f);

        float r = dataArray[(y * cubeInfo.size * stride) + (x * stride) + 0];
        float g = dataArray[(y * cubeInfo.size * stride) + (x * stride) + 1];
        float b = dataArray[(y * cubeInfo.size * stride) + (x * stride) + 2];

        // Handle Integer types.
        if (cubeInfo.type == EngineConstants::TEXTURETYPE_UNSIGNED_INT) {
          r /= 255.f;
          g /= 255.f;
          b /= 255.f;
        }

        // Handle Gamma space textures.
        if (cubeInfo.gammaSpace) {
          r = std::pow(Scalar::Clamp(r), Math::ToLinearSpace);
          g = std::pow(Scalar::Clamp(g), Math::ToLinearSpace);
          b = std::pow(Scalar::Clamp(b), Math::ToLinearSpace);
        }

        Color3 color(r, g, b);

        sphericalHarmonics.addLight(worldDirection, color, deltaSolidAngle);

        totalSolidAngle += deltaSolidAngle;

        u += du;
      }

      v += dv;
    }
  }

  // Solid angle for entire sphere is 4*pi
  float sphereSolidAngle = 4.f * Math::PI;

  // Adjust the solid angle to allow for how many faces we processed.
  float facesProcessed     = 6.f;
  float expectedSolidAngle = sphereSolidAngle * facesProcessed / 6.f;

  // Adjust the harmonics so that the accumulated solid angle matches the
  // expected solid angle. This is needed because the numerical integration over
  // the cube uses a small angle approximation of solid angle for each texel
  // (see deltaSolidAngle), and also to compensate for accumulative error due to
  // float precision in the summation.
  float correctionFactor = expectedSolidAngle / totalSolidAngle;
  sphericalHarmonics.scale(correctionFactor);

  sphericalHarmonics.convertIncidentRadianceToIrradiance();
  sphericalHarmonics.convertIrradianceToLambertianRadiance();

  return std::make_unique<SphericalPolynomial>(
    SphericalPolynomial::FromHarmonics(sphericalHarmonics));
}

} // end of namespace BABYLON
