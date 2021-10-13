#include <babylon/engines/shader_store.h>

namespace BABYLON {

std::string ShaderStore::ShadersRepository = "src/Shaders/";

std::unordered_map<std::string, std::string> ShaderStore::ShadersStore = {};

std::unordered_map<std::string, std::string> ShaderStore::IncludesShadersStore = {};

std::string ShaderStore::ShadersRepositoryWGSL = "src/ShadersWGSL/";

std::unordered_map<std::string, std::string> ShaderStore::ShadersStoreWGSL = {};

std::unordered_map<std::string, std::string> ShaderStore::IncludesShadersStoreWGSL = {};

std::string& ShaderStore::GetShadersRepository(const ShaderLanguage shaderLanguage)
{
  return shaderLanguage == ShaderLanguage::GLSL ? ShaderStore::ShadersRepository :
                                                  ShaderStore::ShadersRepositoryWGSL;
}

std::unordered_map<std::string, std::string>&
ShaderStore::GetShadersStore(const ShaderLanguage shaderLanguage)
{
  return shaderLanguage == ShaderLanguage::GLSL ? ShaderStore::ShadersStore :
                                                  ShaderStore::ShadersStoreWGSL;
}

std::unordered_map<std::string, std::string>&
ShaderStore::GetIncludesShadersStore(const ShaderLanguage shaderLanguage)
{
  return shaderLanguage == ShaderLanguage::GLSL ? ShaderStore::IncludesShadersStore :
                                                  ShaderStore::IncludesShadersStoreWGSL;
}

} // end of namespace BABYLON
