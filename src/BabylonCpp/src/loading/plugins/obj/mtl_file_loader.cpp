#include <babylon/loading/plugins/obj/mtl_file_loader.h>

#include <babylon/engines/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

bool MTLFileLoader::INVERT_TEXTURE_Y = true;

MTLFileLoader::MTLFileLoader()
{
}

void MTLFileLoader::parseMTL(Scene* scene, const std::variant<std::string, ArrayBuffer>& iData,
                             const std::string& rootUrl, bool forAssetContainer)
{
  if (std::holds_alternative<ArrayBuffer>(iData)) {
    return;
  }

  const auto& data = std::get<std::string>(iData);

  // Split the lines from the file
  const auto lines = StringTools::split(data, '\n');
  // Space char
  const auto delimiter_pattern = ' ';
  // Array with RGB colors
  Float32Array color;
  // New material
  StandardMaterialPtr material = nullptr;

  // Look at each line
  for (const auto& line : lines) {
    // Blank line or comment
    if (line.size() == 0 || line[0] == '#') {
      continue;
    }

    // Get the first parameter (keyword)
    auto pos = StringTools::indexOf(line, " ");
    auto key = (pos >= 0) ? line.substr(0, static_cast<size_t>(pos)) : line;
    key      = StringTools::toLowerCase(key);

    // Get the data following the key
    auto substr = line.substr(static_cast<size_t>(pos) + 1);
    auto value  = (pos >= 0) ? StringTools::trim(substr) : std::string("");

    // This mtl keyword will create the new material
    if (key == "newmtl") {
      // Check if it is the first material.
      // Materials specifications are described after this keyword.
      if (material) {
        // Add the previous material in the material array.
        materials.emplace_back(material);
      }
      // Create a new material.
      // value is the name of the material read in the mtl file

      scene->_blockEntityCollection = forAssetContainer;
      material                      = StandardMaterial::New(value, scene);
      scene->_blockEntityCollection = false;
    }
    else if (key == "kd" && material) {
      // Diffuse color (color under white light) using RGB values

      // value  = "r g b"
      color.clear();
      const auto split = StringTools::split(value, delimiter_pattern);
      for (const auto& str : split) {
        color.emplace_back(StringTools::toNumber<float>(str));
      }
      // color = [r,g,b]
      // Set tghe color into the material
      material->diffuseColor = Color3::FromArray(color);
    }
    else if (key == "ka" && material) {
      // Ambient color (color under shadow) using RGB values

      // value = "r g b"
      color.clear();
      const auto split = StringTools::split(value, delimiter_pattern);
      for (const auto& str : split) {
        color.emplace_back(StringTools::toNumber<float>(str));
      }
      // color = [r,g,b]
      // Set tghe color into the material
      material->ambientColor = Color3::FromArray(color);
    }
    else if (key == "ks" && material) {
      // Specular color (color when light is reflected from shiny surface) using RGB values

      // value = "r g b"
      color.clear();
      const auto split = StringTools::split(value, delimiter_pattern);
      for (const auto& str : split) {
        color.emplace_back(StringTools::toNumber<float>(str));
      }
      // color = [r,g,b]
      // Set the color into the material
      material->specularColor = Color3::FromArray(color);
    }
    else if (key == "ke" && material) {
      // Emissive color using RGB values
      color.clear();
      const auto split = StringTools::split(value, delimiter_pattern);
      for (const auto& str : split) {
        color.emplace_back(StringTools::toNumber<float>(str));
      }
      material->emissiveColor = Color3::FromArray(color);
    }
    else if (key == "ns" && material) {

      // value = "Integer"
      material->specularPower = StringTools::toNumber<float>(value);
    }
    else if (key == "d" && material) {
      // d is dissolve for current material. It mean alpha for BABYLON
      material->alpha = StringTools::toNumber<float>(value);

      // Texture
      // This part can be improved by adding the possible options of texture
    }
    else if (key == "map_ka" && material) {
      // ambient texture map with a loaded image
      // We must first get the folder of the image
      material->ambientTexture = MTLFileLoader::_getTexture(rootUrl, value, scene);
    }
    else if (key == "map_kd" && material) {
      // Diffuse texture map with a loaded image
      material->diffuseTexture = MTLFileLoader::_getTexture(rootUrl, value, scene);
    }
    else if (key == "map_ks" && material) {
      // Specular texture map with a loaded image
      // We must first get the folder of the image
      material->specularTexture = MTLFileLoader::_getTexture(rootUrl, value, scene);
    }
    else if (key == "map_ns") {
      // Specular
      // Specular highlight component
      // We must first get the folder of the image
      //
      // Not supported by BABYLON
      //
      //    continue;
    }
    else if (key == "map_bump" && material) {
      // The bump texture
      material->bumpTexture = MTLFileLoader::_getTexture(rootUrl, value, scene);
    }
    else if (key == "map_d" && material) {
      // The dissolve of the material
      material->opacityTexture = MTLFileLoader::_getTexture(rootUrl, value, scene);

      // Options for illumination
    }
    else if (key == "illum") {
      // Illumination
      if (value == "0") {
        // That mean Kd == Kd
      }
      else if (value == "1") {
        // Color on and Ambient on
      }
      else if (value == "2") {
        // Highlight on
      }
      else if (value == "3") {
        // Reflection on and Ray trace on
      }
      else if (value == "4") {
        // Transparency: Glass on, Reflection: Ray trace on
      }
      else if (value == "5") {
        // Reflection: Fresnel on and Ray trace on
      }
      else if (value == "6") {
        // Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
      }
      else if (value == "7") {
        // Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
      }
      else if (value == "8") {
        // Reflection on and Ray trace off
      }
      else if (value == "9") {
        // Transparency: Glass on, Reflection: Ray trace off
      }
      else if (value == "10") {
        // Casts shadows onto invisible surfaces
      }
    }
    else {
      // console.log("Unhandled expression at line : " + i +'\n' + "with value : " + line);
    }
  }
  // At the end of the file, add the last material
  if (material) {
    materials.emplace_back(material);
  }
}

TexturePtr MTLFileLoader::_getTexture(const std::string& rootUrl, const std::string& value,
                                      Scene* scene)
{
  if (value.empty()) {
    return nullptr;
  }

  auto url = rootUrl;
  // Load from input file.
  if (rootUrl == "file:") {
    auto lastDelimiter = StringTools::lastIndexOf(value, "\\");
    if (lastDelimiter == -1) {
      lastDelimiter = StringTools::lastIndexOf(value, "/");
    }

    if (lastDelimiter > -1) {
      url += value.substr(static_cast<size_t>(lastDelimiter) + 1);
    }
    else {
      url += value;
    }
  }
  // Not from input file.
  else {
    url += value;
  }

  return Texture::New(url, scene, false, MTLFileLoader::INVERT_TEXTURE_Y);
}
} // end of namespace BABYLON
