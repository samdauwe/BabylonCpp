#include <babylon/extensions/shaderbuilder/shader.h>

#include <babylon/extensions/shaderbuilder/icolor.h>

namespace BABYLON {
namespace Extensions {

unsigned int Shader::Indexer        = 1;
unsigned int Shader::ShaderIdentity = 0;
ShaderBuilder* Shader::Me           = nullptr;

std::string Shader::Replace(const std::string /*s*/, const std::string /*t*/,
                            const std::string /*d*/)
{
  return "";
}

float Shader::Def(float /*a*/, float /*b*/)
{
  return 0.f;
}

std::string Shader::Def(const std::string& /*a*/, const std::string& /*b*/)
{
  return "";
}

std::string Shader::Join(const std::vector<std::string>& /*s*/)
{
  return "";
}

std::string Shader::Print(int /*i*/)
{
  return "";
}

std::string Shader::Print(float /*i*/)
{
  return "";
}

std::string Shader::Print(const std::string& /*s*/)
{
  return "";
}

std::string Shader::Print(const IColor& /*s*/)
{
  return "";
}

std::string Shader::Custom()
{
  return "";
}

std::string Shader::Index()
{
  return "";
}

std::string Shader::DefCustom(const std::string& /*t*/,
                              const std::string& /*c*/)
{
  return "";
}

IColor Shader::toRGB(int /*a*/, int /*b*/)
{
  return IColor();
}

IColor Shader::torgb(int /*a*/, int /*b*/)
{
  return IColor();
}

float Shader::toID(int /*a*/, int /*b*/)
{
  return 0.f;
}

} // end of namespace Extensions
} // end of namespace BABYLON
