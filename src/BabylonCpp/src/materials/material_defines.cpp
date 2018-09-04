#include <babylon/materials/material_defines.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

MaterialDefines::MaterialDefines()
    : _isDirty{true}
    , _renderId{-1}
    , _areLightsDirty{true}
    , _areAttributesDirty{true}
    , _areTexturesDirty{true}
    , _areFresnelDirty{true}
    , _areMiscDirty{true}
    , _areImageProcessingDirty{true}
    , _normals{false}
    , _uvs{false}
    , _needNormals{false}
    , _needUVs{false}
{
}

MaterialDefines::~MaterialDefines()
{
}

bool MaterialDefines::operator[](const string_t& define) const
{
  return stl_util::contains(boolDef, define) && boolDef.at(define);
}

bool MaterialDefines::operator==(const MaterialDefines& rhs) const
{
  return isEqual(rhs);
}

bool MaterialDefines::operator!=(const MaterialDefines& rhs) const
{
  return !(operator==(rhs));
}

std::ostream& operator<<(std::ostream& os,
                         const MaterialDefines& materialDefines)
{
  for (const auto& item : materialDefines.boolDef) {
    if (item.second) {
      os << "#define " << item.first << "\n";
    }
  }

  for (const auto& item : materialDefines.intDef) {
    os << "#define " << item.first << " " << item.second << "\n";
  }

  for (const auto& item : materialDefines.floatDef) {
    os << "#define " << item.first << " " << item.second << "\n";
  }

  for (const auto& item : materialDefines.stringDef) {
    os << "#define " << item.first << " " << item.second << "\n";
  }

  return os;
}

bool MaterialDefines::isDirty() const
{
  return _isDirty;
}

void MaterialDefines::markAsProcessed()
{
  _isDirty                 = false;
  _areAttributesDirty      = false;
  _areTexturesDirty        = false;
  _areFresnelDirty         = false;
  _areLightsDirty          = false;
  _areMiscDirty            = false;
  _areImageProcessingDirty = false;
}

void MaterialDefines::markAsUnprocessed()
{
  _isDirty = true;
}

void MaterialDefines::markAllAsDirty()
{
  _areTexturesDirty        = true;
  _areAttributesDirty      = true;
  _areLightsDirty          = true;
  _areFresnelDirty         = true;
  _areMiscDirty            = true;
  _areImageProcessingDirty = true;
  _isDirty                 = true;
}

void MaterialDefines::markAsImageProcessingDirty()
{
  _areImageProcessingDirty = true;
  _isDirty                 = true;
}

void MaterialDefines::markAsLightDirty()
{
  _areLightsDirty = true;
  _isDirty        = true;
}

void MaterialDefines::markAsAttributesDirty()
{
  _areAttributesDirty = true;
  _isDirty            = true;
}

void MaterialDefines::markAsTexturesDirty()
{
  _areTexturesDirty = true;
  _isDirty          = true;
}

void MaterialDefines::markAsFresnelDirty()
{
  _areFresnelDirty = true;
  _isDirty         = true;
}

void MaterialDefines::markAsMiscDirty()
{
  _areMiscDirty = true;
  _isDirty      = true;
}

void MaterialDefines::rebuild()
{
}

bool MaterialDefines::isEqual(const MaterialDefines& other) const
{
  if ((boolDef.size() != other.boolDef.size())
      || (intDef.size() != other.intDef.size())
      || (floatDef.size() != other.floatDef.size())
      || (stringDef.size() != other.stringDef.size())) {
    return false;
  }

  if ((_isDirty != other._isDirty) || (_renderId != other._renderId)
      || (_areLightsDirty != other._areLightsDirty)
      || (_areAttributesDirty != other._areAttributesDirty)
      || (_areTexturesDirty != other._areTexturesDirty)
      || (_areFresnelDirty != other._areFresnelDirty)
      || (_areMiscDirty != other._areMiscDirty)
      || (_areImageProcessingDirty != other._areImageProcessingDirty)
      || (_normals != other._normals) || (_uvs != other._uvs)
      || (_needNormals != other._needNormals) || (_needUVs != other._needUVs)) {
    return false;
  }

  if ((boolDef != other.boolDef) || (intDef != other.intDef)
      || (floatDef != other.floatDef) || (stringDef != other.stringDef)) {
    return false;
  }

  return true;
}

void MaterialDefines::cloneTo(MaterialDefines& other)
{
  other._isDirty                 = _isDirty;
  other._renderId                = _renderId;
  other._areLightsDirty          = _areLightsDirty;
  other._areAttributesDirty      = _areAttributesDirty;
  other._areTexturesDirty        = _areTexturesDirty;
  other._areFresnelDirty         = _areFresnelDirty;
  other._areMiscDirty            = _areMiscDirty;
  other._areImageProcessingDirty = _areImageProcessingDirty;
  other._normals                 = _normals;
  other._uvs                     = _uvs;
  other._needNormals             = _needNormals;
  other._needUVs                 = _needUVs;

  other.boolDef   = boolDef;
  other.intDef    = intDef;
  other.floatDef  = floatDef;
  other.stringDef = stringDef;
}

void MaterialDefines::reset()
{
  _isDirty            = true;
  _renderId           = -1;
  _areLightsDirty     = true;
  _areAttributesDirty = true;
  _areTexturesDirty   = true;
  _areFresnelDirty    = true;
  _areMiscDirty       = true;
  _normals            = false;
  _uvs                = false;
  _needNormals        = false;
  _needUVs            = false;
}

string_t MaterialDefines::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

} // end of namespace BABYLON
