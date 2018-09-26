#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_SETTINGS_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_SETTINGS_H

#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/extensions/shaderbuilder/itexture.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT ShaderSetting {
  std::vector<ITexture> Texture2Ds;
  std::vector<ITexture> TextureCubes;
  bool CameraShot;
  std::string PrecisionMode;

  bool Transparency;
  bool Back;
  bool Front;
  bool Wire;
  bool Uv;
  bool Uv2;
  bool Center;
  bool Flags;

  bool FragmentView;
  bool FragmentWorld;
  bool FragmentWorldView;
  bool FragmentViewProjection;

  std::string SpecularMap;
  std::string NormalMap;
  std::string Normal;
  std::string NormalOpacity;

  bool WorldPosition;
  bool Vertex;

  bool VertexView;
  bool VertexWorld;
  bool VertexWorldView;
  bool VertexViewProjection;

  bool Mouse;
  bool Screen;
  bool Camera;
  bool Look;
  bool Time;
  bool GlobalTime;
  bool ReflectMatrix;

  bool Helpers;
}; // end of struct ShaderStruct

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_SETTINGS_H
