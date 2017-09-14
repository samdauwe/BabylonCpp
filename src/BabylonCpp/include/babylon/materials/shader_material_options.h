#ifndef BABYLON_MATERIALS_SHADER_MATERIAL_OPTIONS_H
#define BABYLON_MATERIALS_SHADER_MATERIAL_OPTIONS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ShaderMaterialOptions {
  bool needAlphaBlending        = false;
  bool needAlphaTesting         = false;
  vector_t<string_t> attributes = {"position", "normal", "uv"};
  vector_t<string_t> uniforms   = {"worldViewProjection"};
  vector_t<string_t> uniformBuffers;
  vector_t<string_t> samplers;
  vector_t<string_t> defines;
}; // end of struct ShaderMaterialOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_SHADER_MATERIAL_OPTIONS_H
