#include <babylon/materials/material_defines.h>

namespace BABYLON {

MaterialDefines::MaterialDefines()
    : NUM_BONE_INFLUENCERS{0}
    , BonesPerMesh{0}
    , NUM_MORPH_INFLUENCERS{0}
    , TANGENT{false}
    , SHADOWS{false}
    , LIGHTMAPEXCLUDED{false}
    , USERIGHTHANDEDSYSTEM{false}
    , _isDirty{true}
    , _renderId{-1}
    , _areLightsDirty{true}
    , _areAttributesDirty{true}
    , _areTexturesDirty{true}
    , _areFresnelDirty{true}
    , _areMiscDirty{true}
    , _normals{false}
    , _uvs{false}
    , _needNormals{false}
    , _needUVs{false}
{
}

MaterialDefines::~MaterialDefines()
{
}

bool MaterialDefines::operator[](unsigned int define) const
{
  return (define < defines.size()) ? defines[define] : false;
}

bool MaterialDefines::operator==(const MaterialDefines& rhs) const
{
  return isEqual(rhs);
}

bool MaterialDefines::operator!=(const MaterialDefines& rhs) const
{
  return !(operator==(rhs));
}

void MaterialDefines::resizeLights(unsigned int lightIndex)
{
  if (lightIndex >= lights.size()) {
    for (size_t i = lights.size(); i <= lightIndex; ++i) {
      lights.emplace_back(false);
      pointlights.emplace_back(false);
      dirlights.emplace_back(false);
      hemilights.emplace_back(false);
      spotlights.emplace_back(false);
      shadows.emplace_back(false);
      shadowesms.emplace_back(false);
      shadowpcfs.emplace_back(false);
      lightmapexcluded.emplace_back(false);
      lightmapnospecular.emplace_back(false);
    }
  }
}

std::ostream& operator<<(std::ostream& os,
                         const MaterialDefines& materialDefines)
{
  for (size_t i = 0; i < materialDefines.defines.size(); ++i) {
    if (materialDefines.defines[i]) {
      os << "#define " << materialDefines._keys[i] << "\n";
    }
  }

  os << "#define NUM_BONE_INFLUENCERS " << materialDefines.NUM_BONE_INFLUENCERS
     << "\n";
  os << "#define BonesPerMesh " << materialDefines.BonesPerMesh << "\n";
  os << "#define NUM_MORPH_INFLUENCERS "
     << materialDefines.NUM_MORPH_INFLUENCERS << "\n";
  if (materialDefines.TANGENT) {
    os << "#define TANGENT " << materialDefines.TANGENT << "\n";
  }
  if (materialDefines.SHADOWS) {
    os << "#define SHADOWS " << materialDefines.SHADOWS << "\n";
  }
  if (materialDefines.LIGHTMAPEXCLUDED) {
    os << "#define LIGHTMAPEXCLUDED " << materialDefines.LIGHTMAPEXCLUDED
       << "\n";
  }

  for (size_t i = 0; i < materialDefines.lights.size(); ++i) {
    if (materialDefines.lights[i]) {
      os << "#define LIGHT" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.pointlights.size(); ++i) {
    if (materialDefines.pointlights[i]) {
      os << "#define POINTLIGHT" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.dirlights.size(); ++i) {
    if (materialDefines.dirlights[i]) {
      os << "#define DIRLIGHT" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.hemilights.size(); ++i) {
    if (materialDefines.hemilights[i]) {
      os << "#define HEMILIGHT" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.spotlights.size(); ++i) {
    if (materialDefines.spotlights[i]) {
      os << "#define SPOTLIGHT" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadows.size(); ++i) {
    if (materialDefines.shadows[i]) {
      os << "#define SHADOW" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowesms.size(); ++i) {
    if (materialDefines.shadowesms[i]) {
      os << "#define SHADOWESM" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowpcfs.size(); ++i) {
    if (materialDefines.shadowpcfs[i]) {
      os << "#define SHADOWPCF" << i << "\n";
    }
  }

  return os;
}

bool MaterialDefines::isDirty() const
{
  return _isDirty;
}

void MaterialDefines::markAsProcessed()
{
  _isDirty            = false;
  _areAttributesDirty = false;
  _areTexturesDirty   = false;
  _areFresnelDirty    = false;
  _areLightsDirty     = false;
  _areMiscDirty       = false;
}

void MaterialDefines::markAsUnprocessed()
{
  _isDirty = true;
}

void MaterialDefines::markAllAsDirty()
{
  _areTexturesDirty   = true;
  _areAttributesDirty = true;
  _areLightsDirty     = true;
  _areFresnelDirty    = true;
  _areMiscDirty       = true;
  _isDirty            = true;
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
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }

  NUM_BONE_INFLUENCERS  = 0;
  BonesPerMesh          = 0;
  NUM_MORPH_INFLUENCERS = 0;
  TANGENT               = false;
  SHADOWS               = false;
  LIGHTMAPEXCLUDED      = false;
  USERIGHTHANDEDSYSTEM  = false;
}

bool MaterialDefines::isEqual(const MaterialDefines& other) const
{
  if ((defines.size() != other.defines.size())
      || (_keys.size() != other._keys.size())
      || (lights.size() != other.lights.size())
      || (pointlights.size() != other.pointlights.size())
      || (dirlights.size() != other.dirlights.size())
      || (hemilights.size() != other.hemilights.size())
      || (spotlights.size() != other.spotlights.size())
      || (shadows.size() != other.shadows.size())
      || (shadowesms.size() != other.shadowesms.size())
      || (shadowpcfs.size() != other.shadowpcfs.size())) {
    return false;
  }

  if ((NUM_BONE_INFLUENCERS != other.NUM_BONE_INFLUENCERS)
      || (BonesPerMesh != other.BonesPerMesh)
      || (NUM_MORPH_INFLUENCERS != other.NUM_MORPH_INFLUENCERS)
      || (TANGENT != other.TANGENT) || (SHADOWS != other.SHADOWS)
      || (LIGHTMAPEXCLUDED != other.LIGHTMAPEXCLUDED)
      || (USERIGHTHANDEDSYSTEM != other.USERIGHTHANDEDSYSTEM)
      || (_isDirty != other._isDirty) || (_renderId != other._renderId)
      || (_areLightsDirty != other._areLightsDirty)
      || (_areAttributesDirty != other._areAttributesDirty)
      || (_areTexturesDirty != other._areTexturesDirty)
      || (_areFresnelDirty != other._areFresnelDirty)
      || (_areMiscDirty != other._areMiscDirty) || (_normals != other._normals)
      || (_uvs != other._uvs) || (_needNormals != other._needNormals)
      || (_needUVs != other._needUVs)) {
    return false;
  }

  for (size_t i = 0; i < defines.size(); ++i) {
    if (defines[i] != other.defines[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < _keys.size(); ++i) {
    if (_keys[i] != other._keys[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < lights.size(); ++i) {
    if (lights[i] != other.lights[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < pointlights.size(); ++i) {
    if (pointlights[i] != other.pointlights[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < dirlights.size(); ++i) {
    if (dirlights[i] != other.dirlights[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < hemilights.size(); ++i) {
    if (hemilights[i] != other.hemilights[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < spotlights.size(); ++i) {
    if (spotlights[i] != other.spotlights[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadows.size(); ++i) {
    if (shadows[i] != other.shadows[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowesms.size(); ++i) {
    if (shadowesms[i] != other.shadowesms[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowpcfs.size(); ++i) {
    if (shadowpcfs[i] != other.shadowpcfs[i]) {
      return false;
    }
  }

  return true;
}

void MaterialDefines::cloneTo(MaterialDefines& other)
{
  other.defines = defines;
  other._keys   = _keys;

  other.NUM_BONE_INFLUENCERS  = NUM_BONE_INFLUENCERS;
  other.BonesPerMesh          = BonesPerMesh;
  other.NUM_MORPH_INFLUENCERS = NUM_MORPH_INFLUENCERS;
  other.TANGENT               = TANGENT;
  other.SHADOWS               = SHADOWS;
  other.LIGHTMAPEXCLUDED      = LIGHTMAPEXCLUDED;
  other.USERIGHTHANDEDSYSTEM  = USERIGHTHANDEDSYSTEM;
  other._isDirty              = _isDirty;
  other._renderId             = _renderId;
  other._areLightsDirty       = _areLightsDirty;
  other._areAttributesDirty   = _areAttributesDirty;
  other._areTexturesDirty     = _areTexturesDirty;
  other._areFresnelDirty      = _areFresnelDirty;
  other._areMiscDirty         = _areMiscDirty;
  other._normals              = _normals;
  other._uvs                  = _uvs;
  other._needNormals          = _needNormals;
  other._needUVs              = _needUVs;

  other.lights      = lights;
  other.pointlights = pointlights;
  other.dirlights   = dirlights;
  other.hemilights  = hemilights;
  other.spotlights  = spotlights;
  other.shadows     = shadows;
  other.shadowesms  = shadowesms;
  other.shadowpcfs  = shadowpcfs;
}

void MaterialDefines::reset()
{
  for (size_t i = 0; i < defines.size(); ++i) {
    defines[i] = false;
  }

  NUM_BONE_INFLUENCERS  = 0;
  BonesPerMesh          = 0;
  NUM_MORPH_INFLUENCERS = 0;
  TANGENT               = false;
  SHADOWS               = false;
  LIGHTMAPEXCLUDED      = false;
  USERIGHTHANDEDSYSTEM  = false;
  _isDirty              = true;
  _renderId             = -1;
  _areLightsDirty       = true;
  _areAttributesDirty   = true;
  _areTexturesDirty     = true;
  _areFresnelDirty      = true;
  _areMiscDirty         = true;
  _normals              = false;
  _uvs                  = false;
  _needNormals          = false;
  _needUVs              = false;

  lights.clear();
  pointlights.clear();
  dirlights.clear();
  hemilights.clear();
  spotlights.clear();
  shadows.clear();
  shadowesms.clear();
  shadowpcfs.clear();
}

std::string MaterialDefines::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

} // end of namespace BABYLON
