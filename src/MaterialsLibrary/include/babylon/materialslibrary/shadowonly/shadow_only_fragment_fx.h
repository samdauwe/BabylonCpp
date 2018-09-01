#ifndef BABYLON_MATERIALS_LIBRARY_SHADOWONLY_SHADOW_ONLY_FRAGMENT_FX_H
#define BABYLON_MATERIALS_LIBRARY_SHADOWONLY_SHADOW_ONLY_FRAGMENT_FX_H

namespace BABYLON {

extern const char* shadowOnlyPixelShader;

const char* shadowOnlyPixelShader
  = "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"
    "// Constants\n"
    "uniform vec3 vEyePosition;\n"
    "uniform float alpha;\n"
    "uniform vec3 shadowColor;\n"
    "\n"
    "// Input\n"
    "varying vec3 vPositionW;\n"
    "\n"
    "#ifdef NORMAL\n"
    "varying vec3 vNormalW;\n"
    "#endif\n"
    "\n"
    "// Helper functions\n"
    "#include<helperFunctions>\n"
    "\n"
    "// Lights\n"
    "#include<__decl__lightFragment>[0..maxSimultaneousLights]\n"
    "\n"
    "#include<lightsFragmentFunctions>\n"
    "#include<shadowsFragmentFunctions>\n"
    "\n"
    "#include<clipPlaneFragmentDeclaration>\n"
    "\n"
    "// Fog\n"
    "#include<fogFragmentDeclaration>\n"
    "\n"
    "void main(void) {\n"
    "#include<clipPlaneFragment>\n"
    "\n"
    "  vec3 viewDirectionW = normalize(vEyePosition - vPositionW);\n"
    "\n"
    "  // Normal\n"
    "#ifdef NORMAL\n"
    "  vec3 normalW = normalize(vNormalW);\n"
    "#else\n"
    "  vec3 normalW = vec3(1.0, 1.0, 1.0);\n"
    "#endif\n"
    "\n"
    "  // Lighting\n"
    "  vec3 diffuseBase = vec3(0., 0., 0.);\n"
    "  lightingInfo info;\n"
    "  float shadow = 1.;\n"
    "  float glossiness = 0.;\n"
    "  \n"
    "#include<lightFragment>[0..1]\n"
    "\n"
    "  // Composition\n"
    "  vec4 color = vec4(shadowColor, (1.0 - clamp(shadow, 0., 1.)) * alpha);\n"
    "\n"
    "#include<fogFragment>\n"
    "\n"
    "  gl_FragColor = color;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_SHADOWONLY_SHADOW_ONLY_FRAGMENT_FX_H
