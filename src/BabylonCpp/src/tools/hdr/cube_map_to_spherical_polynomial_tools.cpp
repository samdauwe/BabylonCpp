#include <babylon/tools/hdr/cube_map_to_spherical_polynomial_tools.h>

#include <babylon/math/color3.h>
#include <babylon/math/spherical_harmonics.h>
#include <babylon/math/spherical_polynomial.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Internals {

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

SphericalPolynomial
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
    const FileFaceOrientation& fileFace = FileFaces[faceIndex];
    Float32Array dataArray;
    if (fileFace.name == "right") {
      dataArray = cubeInfo.right;
    }
    else if (fileFace.name == "left") {
      dataArray = cubeInfo.left;
    }
    else if (fileFace.name == "up") {
      dataArray = cubeInfo.up;
    }
    else if (fileFace.name == "down") {
      dataArray = cubeInfo.down;
    }
    else if (fileFace.name == "front") {
      dataArray = cubeInfo.front;
    }
    else if (fileFace.name == "back") {
      dataArray = cubeInfo.back;
    }
    float v = minUV;

    // TODO: we could perform the summation directly into a SphericalPolynomial
    // (SP), which is more efficient than SphericalHarmonic (SH).
    // This is possible because during the summation we do not need the
    // SH-specific properties, e.g. orthogonality.
    // Because SP is still linear, so summation is fine in that basis.

    for (size_t y = 0; y < cubeInfo.size; ++y) {
      float u = minUV;

      for (size_t x = 0; x < cubeInfo.size; ++x) {
        // World direction (not normalised)
        Vector3 worldDirection = fileFace.worldAxisForFileX.scale(u)
                                   .add(fileFace.worldAxisForFileY.scale(v))
                                   .add(fileFace.worldAxisForNormal);
        worldDirection.normalize();

        float deltaSolidAngle = std::pow(1.f + u * u + v * v, -3.f / 2.f);

        if (/* DISABLES CODE */ (true)) {
          float r = dataArray[(y * cubeInfo.size * 3) + (x * 3) + 0];
          float g = dataArray[(y * cubeInfo.size * 3) + (x * 3) + 1];
          float b = dataArray[(y * cubeInfo.size * 3) + (x * 3) + 2];

          Color3 color(r, g, b);

          sphericalHarmonics.addLight(worldDirection, color, deltaSolidAngle);
        }
        else {
          if (faceIndex == 0) {
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 0] = 1;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 1] = 0;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 2] = 0;
          }
          else if (faceIndex == 1) {
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 0] = 0;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 1] = 1;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 2] = 0;
          }
          else if (faceIndex == 2) {
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 0] = 0;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 1] = 0;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 2] = 1;
          }
          else if (faceIndex == 3) {
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 0] = 1;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 1] = 1;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 2] = 0;
          }
          else if (faceIndex == 4) {
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 0] = 1;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 1] = 0;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 2] = 1;
          }
          else if (faceIndex == 5) {
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 0] = 0;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 1] = 1;
            dataArray[(y * cubeInfo.size * 3) + (x * 3) + 2] = 1;
          }

          Color3 color(dataArray[(y * cubeInfo.size * 3) + (x * 3) + 0],
                       dataArray[(y * cubeInfo.size * 3) + (x * 3) + 1],
                       dataArray[(y * cubeInfo.size * 3) + (x * 3) + 2]);

          sphericalHarmonics.addLight(worldDirection, color, deltaSolidAngle);
        }

        totalSolidAngle += deltaSolidAngle;

        u += du;
      }

      v += dv;
    }
  }

  float correctSolidAngle
    = 4.f * Math::PI; // Solid angle for entire sphere is 4*pi
  float correction = correctSolidAngle / totalSolidAngle;

  sphericalHarmonics.scale(correction);

  // Additionally scale by pi -- audit needed
  sphericalHarmonics.scale(1.f / Math::PI);

  return SphericalPolynomial::getSphericalPolynomialFromHarmonics(
    sphericalHarmonics);
}

} // end of namespace Internals
} // end of namespace BABYLON
