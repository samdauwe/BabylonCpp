#ifndef BABYLON_MATERIALS_SHADER_MATERIAL_OPTIONS_H
#define BABYLON_MATERIALS_SHADER_MATERIAL_OPTIONS_H

#include <string>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ShaderMaterialOptions {
  bool needAlphaBlending              = false;
  bool needAlphaTesting               = false;
  std::vector<std::string> attributes = {"position", "normal", "uv"};
  std::vector<std::string> uniforms   = {"worldViewProjection"};
  std::vector<std::string> uniformBuffers;
  std::vector<std::string> samplers;
  std::vector<std::string> defines;
}; // end of struct ShaderMaterialOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_SHADER_MATERIAL_OPTIONS_H
