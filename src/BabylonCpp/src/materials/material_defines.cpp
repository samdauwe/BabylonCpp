#include <babylon/materials/material_defines.h>

namespace BABYLON {

MaterialDefines::MaterialDefines()
    : NUM_BONE_INFLUENCERS{0}
    , BonesPerMesh{0}
    , NUM_MORPH_INFLUENCERS{0}
    , DIFFUSEDIRECTUV{0}
    , AMBIENTDIRECTUV{0}
    , OPACITYDIRECTUV{0}
    , EMISSIVEDIRECTUV{0}
    , SPECULARDIRECTUV{0}
    , BUMPDIRECTUV{0}
    , LIGHTMAPDIRECTUV{0}
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
      shadowcloseesms.emplace_back(false);
      shadowpcsss.emplace_back(false);
      shadowpcfs.emplace_back(false);
      shadowpoissons.emplace_back(false);
      shadowcubes.emplace_back(false);
      projectedLightTexture.emplace_back(false);
      shadowlowqualities.emplace_back(false);
      shadowmediumqualities.emplace_back(false);
      shadowhighqualities.emplace_back(false);
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

  for (size_t i = 0; i < materialDefines.shadowcloseesms.size(); ++i) {
    if (materialDefines.shadowcloseesms[i]) {
      os << "#define SHADOWCLOSEESM" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowpcsss.size(); ++i) {
    if (materialDefines.shadowpcsss[i]) {
      os << "#define SHADOWPCSS" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowpcfs.size(); ++i) {
    if (materialDefines.shadowpcfs[i]) {
      os << "#define SHADOWPCF" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowpoissons.size(); ++i) {
    if (materialDefines.shadowpoissons[i]) {
      os << "#define SHADOWPOISSON" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowcubes.size(); ++i) {
    if (materialDefines.shadowcubes[i]) {
      os << "#define SHADOWCUBE" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.projectedLightTexture.size(); ++i) {
    if (materialDefines.projectedLightTexture[i]) {
      os << "#define PROJECTEDLIGHTTEXTURE" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowlowqualities.size(); ++i) {
    if (materialDefines.shadowlowqualities[i]) {
      os << "#define SHADOWLOWQUALITY" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowmediumqualities.size(); ++i) {
    if (materialDefines.shadowmediumqualities[i]) {
      os << "#define SHADOWMEDIUMQUALITY" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowhighqualities.size(); ++i) {
    if (materialDefines.shadowhighqualities[i]) {
      os << "#define SHADOWHIGHQUALITY" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.lightmapexcluded.size(); ++i) {
    if (materialDefines.lightmapexcluded[i]) {
      os << "#define LIGHTMAPEXCLUDED" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.lightmapnospecular.size(); ++i) {
    if (materialDefines.lightmapnospecular[i]) {
      os << "#define LIGHTMAPNOSPECULAR" << i << "\n";
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
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }

  NUM_BONE_INFLUENCERS  = 0;
  BonesPerMesh          = 0;
  NUM_MORPH_INFLUENCERS = 0;
  DIFFUSEDIRECTUV       = 0;
  AMBIENTDIRECTUV       = 0;
  OPACITYDIRECTUV       = 0;
  EMISSIVEDIRECTUV      = 0;
  SPECULARDIRECTUV      = 0;
  BUMPDIRECTUV          = 0;
  LIGHTMAPDIRECTUV      = 0;
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
      || (shadowcloseesms.size() != other.shadowcloseesms.size())
      || (shadowpcsss.size() != other.shadowpcsss.size())
      || (shadowpcfs.size() != other.shadowpcfs.size())
      || (shadowpoissons.size() != other.shadowpoissons.size())
      || (shadowcubes.size() != other.shadowcubes.size())
      || (projectedLightTexture.size() != other.projectedLightTexture.size())
      || (shadowlowqualities.size() != other.shadowlowqualities.size())
      || (shadowmediumqualities.size() != other.shadowmediumqualities.size())
      || (shadowhighqualities.size() != other.shadowhighqualities.size())
      || (lightmapexcluded.size() != other.lightmapexcluded.size())
      || (lightmapnospecular.size() != other.lightmapnospecular.size())) {
    return false;
  }

  if ((NUM_BONE_INFLUENCERS != other.NUM_BONE_INFLUENCERS)
      || (BonesPerMesh != other.BonesPerMesh)
      || (NUM_MORPH_INFLUENCERS != other.NUM_MORPH_INFLUENCERS)
      || (DIFFUSEDIRECTUV != other.DIFFUSEDIRECTUV)
      || (AMBIENTDIRECTUV != other.AMBIENTDIRECTUV)
      || (OPACITYDIRECTUV != other.OPACITYDIRECTUV)
      || (EMISSIVEDIRECTUV != other.EMISSIVEDIRECTUV)
      || (SPECULARDIRECTUV != other.SPECULARDIRECTUV)
      || (BUMPDIRECTUV != other.BUMPDIRECTUV)
      || (LIGHTMAPDIRECTUV != other.LIGHTMAPDIRECTUV)
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

  for (size_t i = 0; i < shadowcloseesms.size(); ++i) {
    if (shadowcloseesms[i] != other.shadowcloseesms[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowpcsss.size(); ++i) {
    if (shadowpcsss[i] != other.shadowpcsss[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowpcfs.size(); ++i) {
    if (shadowpcfs[i] != other.shadowpcfs[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowpoissons.size(); ++i) {
    if (shadowpoissons[i] != other.shadowpoissons[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowcubes.size(); ++i) {
    if (shadowcubes[i] != other.shadowcubes[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < projectedLightTexture.size(); ++i) {
    if (projectedLightTexture[i] != other.projectedLightTexture[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowlowqualities.size(); ++i) {
    if (shadowlowqualities[i] != other.shadowlowqualities[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowmediumqualities.size(); ++i) {
    if (shadowmediumqualities[i] != other.shadowmediumqualities[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowhighqualities.size(); ++i) {
    if (shadowhighqualities[i] != other.shadowhighqualities[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < lightmapexcluded.size(); ++i) {
    if (lightmapexcluded[i] != other.lightmapexcluded[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < lightmapnospecular.size(); ++i) {
    if (lightmapnospecular[i] != other.lightmapnospecular[i]) {
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
  other.DIFFUSEDIRECTUV       = DIFFUSEDIRECTUV;
  other.AMBIENTDIRECTUV       = AMBIENTDIRECTUV;
  other.OPACITYDIRECTUV       = OPACITYDIRECTUV;
  other.EMISSIVEDIRECTUV      = EMISSIVEDIRECTUV;
  other.SPECULARDIRECTUV      = SPECULARDIRECTUV;
  other.BUMPDIRECTUV          = BUMPDIRECTUV;
  other.LIGHTMAPDIRECTUV      = LIGHTMAPDIRECTUV;
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

  other.lights                = lights;
  other.pointlights           = pointlights;
  other.dirlights             = dirlights;
  other.hemilights            = hemilights;
  other.spotlights            = spotlights;
  other.shadows               = shadows;
  other.shadowesms            = shadowesms;
  other.shadowcloseesms       = shadowcloseesms;
  other.shadowpcsss           = shadowpcsss;
  other.shadowpcfs            = shadowpcfs;
  other.shadowpoissons        = shadowpoissons;
  other.shadowcubes           = shadowcubes;
  other.projectedLightTexture = projectedLightTexture;
  other.shadowlowqualities    = shadowlowqualities;
  other.shadowmediumqualities = shadowmediumqualities;
  other.shadowhighqualities   = shadowhighqualities;
  other.lightmapexcluded      = lightmapexcluded;
  other.lightmapnospecular    = lightmapnospecular;
}

void MaterialDefines::reset()
{
  for (size_t i = 0; i < defines.size(); ++i) {
    defines[i] = false;
  }

  NUM_BONE_INFLUENCERS  = 0;
  BonesPerMesh          = 0;
  NUM_MORPH_INFLUENCERS = 0;
  DIFFUSEDIRECTUV       = 0;
  AMBIENTDIRECTUV       = 0;
  OPACITYDIRECTUV       = 0;
  EMISSIVEDIRECTUV      = 0;
  SPECULARDIRECTUV      = 0;
  BUMPDIRECTUV          = 0;
  LIGHTMAPDIRECTUV      = 0;
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
  shadowcloseesms.clear();
  shadowpcsss.clear();
  shadowpcfs.clear();
  shadowpoissons.clear();
  shadowcubes.clear();
  projectedLightTexture.clear();
  shadowlowqualities.clear();
  shadowmediumqualities.clear();
  shadowhighqualities.clear();
  lightmapexcluded.clear();
  lightmapnospecular.clear();
}

string_t MaterialDefines::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

} // end of namespace BABYLON
