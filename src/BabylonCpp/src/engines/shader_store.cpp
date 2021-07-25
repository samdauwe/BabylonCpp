#include <babylon/engines/shader_store.h>

namespace BABYLON {

std::string ShaderStore::ShadersRepository = "src/Shaders/";

std::unordered_map<std::string, std::string> ShaderStore::ShadersStore = {};

std::unordered_map<std::string, std::string> ShaderStore::IncludesShadersStore = {};

} // end of namespace BABYLON
