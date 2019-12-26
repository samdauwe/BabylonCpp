#include <babylon/misc/highdynamicrange/cube_map_to_spherical_polynomial_tools.h>

#include <babylon/engines/constants.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/spherical_harmonics.h>
#include <babylon/maths/spherical_polynomial.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

std::array<FileFaceOrientation, 6> CubeMapToSphericalPolynomialTools::FileFaces = {{
  FileFaceOrientation("right", Vector3(1, 0, 0), Vector3(0, 0, -1), Vector3(0, -1, 0)), // +X east
  FileFaceOrientation("left", Vector3(-1, 0, 0), Vector3(0, 0, 1), Vector3(0, -1, 0)),  // -X west
  FileFaceOrientation("up", Vector3(0, 1, 0), Vector3(1, 0, 0), Vector3(0, 0, 1)),      // +Y north
  FileFaceOrientation("down", Vector3(0, -1, 0), Vector3(1, 0, 0), Vector3(0, 0, -1)),  // -Y south
  FileFaceOrientation("front", Vector3(0, 0, 1), Vector3(1, 0, 0), Vector3(0, -1, 0)),  // +Z top
  FileFaceOrientation("back", Vector3(0, 0, -1), Vector3(-1, 0, 0), Vector3(0, -1, 0))  // -Z bottom
}};

SphericalPolynomialPtr
CubeMapToSphericalPolynomialTools::ConvertCubeMapTextureToSphericalPolynomial(BaseTexture& texture)
{
  if (!texture.isCube) {
    // Only supports cube Textures currently.
    return nullptr;
  }

  auto size  = static_cast<std::size_t>(texture.getSize().width);
  auto right = texture.readPixels(0);
  auto left  = texture.readPixels(1);

  ArrayBufferView up;
  ArrayBufferView down;
  if (texture.isRenderTarget) {
    up   = texture.readPixels(3);
    down = texture.readPixels(2);
  }
  else {
    up   = texture.readPixels(2);
    down = texture.readPixels(3);
  }

  auto front = texture.readPixels(4);
  auto back  = texture.readPixels(5);

  auto gammaSpace = texture.gammaSpace;
  // Always read as RGBA.
  auto format = Constants::TEXTUREFORMAT_RGBA;
  auto type   = Constants::TEXTURETYPE_UNSIGNED_INT;
  if (texture.textureType() != Constants::TEXTURETYPE_UNSIGNED_INT) {
    type = Constants::TEXTURETYPE_FLOAT;
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

SphericalPolynomialPtr
CubeMapToSphericalPolynomialTools::ConvertCubeMapToSphericalPolynomial(const CubeMapInfo& cubeInfo)
{
  SphericalHarmonics sphericalHarmonics;
  auto totalSolidAngle = 0.f;

  // The (u,v) range is [-1,+1], so the distance between each texel is 2/Size.
  auto du = 2.f / static_cast<float>(cubeInfo.size);
  auto dv = du;

  // The (u,v) of the first texel is half a texel from the corner (-1,-1).
  auto minUV = du * 0.5f - 1.f;

  for (auto faceIndex = 0u; faceIndex < 6; ++faceIndex) {
    const auto& fileFace = FileFaces[faceIndex];
    const auto dataArray = cubeInfo[fileFace.name].float32Array();
    auto v               = minUV;

    // TODO: we could perform the summation directly into a SphericalPolynomial
    // (SP), which is more efficient than SphericalHarmonic (SH).
    // This is possible because during the summation we do not need the
    // SH-specific properties, e.g. orthogonality.
    // Because SP is still linear, so summation is fine in that basis.
    const auto stride = cubeInfo.format == Constants::TEXTUREFORMAT_RGBA ? 4u : 3u;
    for (auto y = 0ull; y < cubeInfo.size; ++y) {
      auto u = minUV;

      for (size_t x = 0; x < cubeInfo.size; ++x) {
        // World direction (not normalised)
        auto worldDirection = fileFace.worldAxisForFileX.scale(u)
                                .add(fileFace.worldAxisForFileY.scale(v))
                                .add(fileFace.worldAxisForNormal);
        worldDirection.normalize();

        auto deltaSolidAngle = std::pow(1.f + u * u + v * v, -3.f / 2.f);

        auto r = dataArray[(y * cubeInfo.size * stride) + (x * stride) + 0];
        auto g = dataArray[(y * cubeInfo.size * stride) + (x * stride) + 1];
        auto b = dataArray[(y * cubeInfo.size * stride) + (x * stride) + 2];

        // Prevent NaN harmonics with extreme HDRI data.
        if (isNaN(r)) {
          r = 0.f;
        }
        if (isNaN(g)) {
          g = 0.f;
        }
        if (isNaN(b)) {
          b = 0.f;
        }

        // Handle Integer types.
        if (cubeInfo.type == Constants::TEXTURETYPE_UNSIGNED_INT) {
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

        // Prevent to explode in case of really high dynamic ranges.
        // sh 3 would not be enough to accurately represent it.
        const auto max = 4096.f;
        r              = Scalar::Clamp(r, 0.f, max);
        g              = Scalar::Clamp(g, 0.f, max);
        b              = Scalar::Clamp(b, 0.f, max);

        Color3 color(r, g, b);

        sphericalHarmonics.addLight(worldDirection, color, deltaSolidAngle);

        totalSolidAngle += deltaSolidAngle;

        u += du;
      }

      v += dv;
    }
  }

  // Solid angle for entire sphere is 4*pi
  auto sphereSolidAngle = 4.f * Math::PI;

  // Adjust the solid angle to allow for how many faces we processed.
  auto facesProcessed     = 6.f;
  auto expectedSolidAngle = sphereSolidAngle * facesProcessed / 6.f;

  // Adjust the harmonics so that the accumulated solid angle matches the
  // expected solid angle. This is needed because the numerical integration over
  // the cube uses a small angle approximation of solid angle for each texel
  // (see deltaSolidAngle), and also to compensate for accumulative error due to
  // float precision in the summation.
  auto correctionFactor = expectedSolidAngle / totalSolidAngle;
  sphericalHarmonics.scaleInPlace(correctionFactor);

  sphericalHarmonics.convertIncidentRadianceToIrradiance();
  sphericalHarmonics.convertIrradianceToLambertianRadiance();

  return std::make_shared<SphericalPolynomial>(
    SphericalPolynomial::FromHarmonics(sphericalHarmonics));
}

} // end of namespace BABYLON
