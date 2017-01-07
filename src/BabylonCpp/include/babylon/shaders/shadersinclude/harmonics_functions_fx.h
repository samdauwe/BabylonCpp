#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_HARMONICS_FUNCTIONS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_HARMONICS_FUNCTIONS_FX_H

namespace BABYLON {

extern const char* harmonicsFunctions;

const char* harmonicsFunctions
  = "#ifdef USESPHERICALFROMREFLECTIONMAP\n"
    "  uniform vec3 vSphericalX;\n"
    "  uniform vec3 vSphericalY;\n"
    "  uniform vec3 vSphericalZ;\n"
    "  uniform vec3 vSphericalXX;\n"
    "  uniform vec3 vSphericalYY;\n"
    "  uniform vec3 vSphericalZZ;\n"
    "  uniform vec3 vSphericalXY;\n"
    "  uniform vec3 vSphericalYZ;\n"
    "  uniform vec3 vSphericalZX;\n"
    "\n"
    "  vec3 EnvironmentIrradiance(vec3 normal)\n"
    "  {\n"
    "  // Note: 'normal' is assumed to be normalised (or near normalised)\n"
    "  // This isn't as critical as it is with other calculations (e.g. specular highlight), but the result will be incorrect nonetheless.\n"
    "\n"
    "  // TODO: switch to optimal implementation\n"
    "  vec3 result =\n"
    "  vSphericalX * normal.x +\n"
    "  vSphericalY * normal.y +\n"
    "  vSphericalZ * normal.z +\n"
    "  vSphericalXX * normal.x * normal.x +\n"
    "  vSphericalYY * normal.y * normal.y +\n"
    "  vSphericalZZ * normal.z * normal.z +\n"
    "  vSphericalYZ * normal.y * normal.z +\n"
    "  vSphericalZX * normal.z * normal.x +\n"
    "  vSphericalXY * normal.x * normal.y;\n"
    "\n"
    "  return result.rgb;\n"
    "  }\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_HARMONICS_FUNCTIONS_FX_H
