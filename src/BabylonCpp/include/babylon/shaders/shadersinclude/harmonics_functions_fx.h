#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_HARMONICS_FUNCTIONS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_HARMONICS_FUNCTIONS_FX_H

namespace BABYLON {

extern const char* harmonicsFunctions;

const char* harmonicsFunctions
  = "#ifdef USESPHERICALFROMREFLECTIONMAP\n"
    "  uniform vec3 vSphericalX;\n"
    "  uniform vec3 vSphericalY;\n"
    "  uniform vec3 vSphericalZ;\n"
    "  uniform vec3 vSphericalXX_ZZ;\n"
    "  uniform vec3 vSphericalYY_ZZ;\n"
    "  uniform vec3 vSphericalZZ;\n"
    "  uniform vec3 vSphericalXY;\n"
    "  uniform vec3 vSphericalYZ;\n"
    "  uniform vec3 vSphericalZX;\n"
    "\n"
    "  vec3 quaternionVectorRotation_ScaledSqrtTwo(vec4 Q, vec3 V){\n"
    "  vec3 T = cross(Q.xyz, V);\n"
    "  T += Q.www * V;\n"
    "  return cross(Q.xyz, T) + V;\n"
    "  }\n"
    "\n"
    "  vec3 environmentIrradianceJones(vec3 normal)\n"
    "  {\n"
    "  // Fast method for evaluating a fixed spherical harmonics function on the sphere (e.g. irradiance or radiance).\n"
    "  // Cost: 24 scalar operations on modern GPU \"scalar\" shader core, or 8 multiply-adds of 3D vectors:\n"
    "  // \"Function Cost 24  24x mad\"\n"
    "\n"
    "  // Note: the lower operation count compared to other methods (e.g. Sloan) is by further\n"
    "  // taking advantage of the input 'normal' being normalised, which affords some further algebraic simplification.\n"
    "  // Namely, the SH coefficients are first converted to spherical polynomial (SP) basis, then \n"
    "  // a substitution is performed using Z^2 = (1 - X^2 - Y^2).\n"
    "\n"
    "  // As with other methods for evaluation spherical harmonic, the input 'normal' is assumed to be normalised (or near normalised).\n"
    "  // This isn't as critical as it is with other calculations (e.g. specular highlight), but the result will be slightly incorrect nonetheless.\n"
    "  float Nx = normal.x;\n"
    "  float Ny = normal.y;\n"
    "  float Nz = normal.z;\n"
    "\n"
    "  vec3 C1 = vSphericalZZ.rgb;\n"
    "  vec3 Cx = vSphericalX.rgb;\n"
    "  vec3 Cy = vSphericalY.rgb;\n"
    "  vec3 Cz = vSphericalZ.rgb;\n"
    "  vec3 Cxx_zz = vSphericalXX_ZZ.rgb;\n"
    "  vec3 Cyy_zz = vSphericalYY_ZZ.rgb;\n"
    "  vec3 Cxy = vSphericalXY.rgb;\n"
    "  vec3 Cyz = vSphericalYZ.rgb;\n"
    "  vec3 Czx = vSphericalZX.rgb;\n"
    "\n"
    "  vec3 a1 = Cyy_zz * Ny + Cy;\n"
    "  vec3 a2 = Cyz * Nz + a1;\n"
    "  vec3 b1 = Czx * Nz + Cx;\n"
    "  vec3 b2 = Cxy * Ny + b1;\n"
    "  vec3 b3 = Cxx_zz * Nx + b2;\n"
    "  vec3 t1 = Cz  * Nz + C1;\n"
    "  vec3 t2 = a2  * Ny + t1;\n"
    "  vec3 t3 = b3  * Nx + t2;\n"
    "\n"
    "  return t3;\n"
    "  }\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_HARMONICS_FUNCTIONS_FX_H
