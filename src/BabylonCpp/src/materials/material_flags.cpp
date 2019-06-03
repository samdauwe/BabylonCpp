#include <babylon/materials/material_flags.h>

#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>

namespace BABYLON {

bool MaterialFlags::_DiffuseTextureEnabled       = true;
bool MaterialFlags::_AmbientTextureEnabled       = true;
bool MaterialFlags::_OpacityTextureEnabled       = true;
bool MaterialFlags::_ReflectionTextureEnabled    = true;
bool MaterialFlags::_EmissiveTextureEnabled      = true;
bool MaterialFlags::_SpecularTextureEnabled      = true;
bool MaterialFlags::_BumpTextureEnabled          = true;
bool MaterialFlags::_LightmapTextureEnabled      = true;
bool MaterialFlags::_RefractionTextureEnabled    = true;
bool MaterialFlags::_ColorGradingTextureEnabled  = true;
bool MaterialFlags::_FresnelEnabled              = true;
bool MaterialFlags::_ClearCoatTextureEnabled     = true;
bool MaterialFlags::_ClearCoatBumpTextureEnabled = true;
bool MaterialFlags::_ClearCoatTintTextureEnabled = true;
bool MaterialFlags::_SheenTextureEnabled         = true;
bool MaterialFlags::_AnisotropicTextureEnabled   = true;
bool MaterialFlags::_ThicknessTextureEnabled     = true;

bool MaterialFlags::DiffuseTextureEnabled()
{
  return _DiffuseTextureEnabled;
}

void MaterialFlags::setDiffuseTextureEnabled(bool value)
{
  if (_DiffuseTextureEnabled == value) {
    return;
  }

  _DiffuseTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::AmbientTextureEnabled()
{
  return _AmbientTextureEnabled;
}

void MaterialFlags::setAmbientTextureEnabled(bool value)
{
  if (_AmbientTextureEnabled == value) {
    return;
  }

  _AmbientTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::OpacityTextureEnabled()
{
  return _OpacityTextureEnabled;
}

void MaterialFlags::setOpacityTextureEnabled(bool value)
{
  if (_OpacityTextureEnabled == value) {
    return;
  }

  _OpacityTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::ReflectionTextureEnabled()
{
  return _ReflectionTextureEnabled;
}

void MaterialFlags::setReflectionTextureEnabled(bool value)
{
  if (_ReflectionTextureEnabled == value) {
    return;
  }

  _ReflectionTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}
bool MaterialFlags::EmissiveTextureEnabled()
{
  return _EmissiveTextureEnabled;
}

void MaterialFlags::setEmissiveTextureEnabled(bool value)
{
  if (_EmissiveTextureEnabled == value) {
    return;
  }

  _EmissiveTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::SpecularTextureEnabled()
{
  return _SpecularTextureEnabled;
}

void MaterialFlags::setSpecularTextureEnabled(bool value)
{
  if (_SpecularTextureEnabled == value) {
    return;
  }

  _SpecularTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::BumpTextureEnabled()
{
  return _BumpTextureEnabled;
}

void MaterialFlags::setBumpTextureEnabled(bool value)
{
  if (_BumpTextureEnabled == value) {
    return;
  }

  _BumpTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::LightmapTextureEnabled()
{
  return _LightmapTextureEnabled;
}

void MaterialFlags::setLightmapTextureEnabled(bool value)
{
  if (_LightmapTextureEnabled == value) {
    return;
  }

  _LightmapTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::RefractionTextureEnabled()
{
  return _RefractionTextureEnabled;
}

void MaterialFlags::setRefractionTextureEnabled(bool value)
{
  if (_RefractionTextureEnabled == value) {
    return;
  }

  _RefractionTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::ColorGradingTextureEnabled()
{
  return _ColorGradingTextureEnabled;
}

void MaterialFlags::setColorGradingTextureEnabled(bool value)
{
  if (_ColorGradingTextureEnabled == value) {
    return;
  }

  _ColorGradingTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::FresnelEnabled()
{
  return _FresnelEnabled;
}

void MaterialFlags::setFresnelEnabled(bool value)
{
  if (_FresnelEnabled == value) {
    return;
  }

  _FresnelEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_FresnelDirtyFlag);
}

bool MaterialFlags::ClearCoatTextureEnabled()
{
  return _ClearCoatTextureEnabled;
}

void MaterialFlags::setClearCoatTextureEnabled(bool value)
{
  if (_ClearCoatTextureEnabled == value) {
    return;
  }

  _ClearCoatTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::ClearCoatBumpTextureEnabled()
{
  return _ClearCoatBumpTextureEnabled;
}

void MaterialFlags::setClearCoatBumpTextureEnabled(bool value)
{
  if (_ClearCoatBumpTextureEnabled == value) {
    return;
  }

  _ClearCoatBumpTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::ClearCoatTintTextureEnabled()
{
  return _ClearCoatTintTextureEnabled;
}

void MaterialFlags::setClearCoatTintTextureEnabled(bool value)
{
  if (_ClearCoatTintTextureEnabled == value) {
    return;
  }

  _ClearCoatTintTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::SheenTextureEnabled()
{
  return _SheenTextureEnabled;
}

void MaterialFlags::setSheenTextureEnabled(bool value)
{
  if (_SheenTextureEnabled == value) {
    return;
  }

  _SheenTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::AnisotropicTextureEnabled()
{
  return _AnisotropicTextureEnabled;
}

void MaterialFlags::setAnisotropicTextureEnabled(bool value)
{
  if (_AnisotropicTextureEnabled == value) {
    return;
  }

  _AnisotropicTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool MaterialFlags::ThicknessTextureEnabled()
{
  return _ThicknessTextureEnabled;
}

void MaterialFlags::setThicknessTextureEnabled(bool value)
{
  if (_ThicknessTextureEnabled == value) {
    return;
  }

  _ThicknessTextureEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

} // end of namespace BABYLON
