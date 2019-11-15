#include <babylon/engines/webgl/webgl2_shader_processor.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>

namespace BABYLON {

WebGL2ShaderProcessor::~WebGL2ShaderProcessor() = default;

std::string WebGL2ShaderProcessor::attributeProcessor(const std::string& attribute)
{
  return String::replace(attribute, "attribute", "in");
}

std::string WebGL2ShaderProcessor::varyingProcessor(const std::string& varying, bool isFragment)
{
  return String::replace(varying, "varying", isFragment ? "in" : "out");
}

std::string WebGL2ShaderProcessor::postProcessor(std::string code,
                                                 const std::vector<std::string>& defines,
                                                 bool isFragment)
{
  const auto hasDrawBuffersExtension
    = String::contains(code, "#extension.+GL_EXT_draw_buffers.+require");

  // Remove extensions
  const std::string regex(
    "#extension.+(GL_OVR_multiview2|GL_OES_standard_derivatives|GL_EXT_shader_texture_lod|GL_EXT_"
    "frag_depth|GL_EXT_draw_buffers).+(enable|require)");
  code = String::regexReplace(code, regex, "");

  // Replace instructions
  code = String::regexReplace(code, "texture2D\\s*\\(", "texture(");
  if (isFragment) {
    code = String::regexReplace(code, "texture2DLodEXT\\s*\\(", "textureLod(");
    code = String::regexReplace(code, "textureCubeLodEXT\\s*\\(", "textureLod(");
    code = String::regexReplace(code, "textureCube\\s*\\(", "texture(");
    code = String::regexReplace(code, "gl_FragDepthEXT", "gl_FragDepth");
    code = String::regexReplace(code, "gl_FragColor", "glFragColor");
    code = String::regexReplace(code, "gl_FragData", "glFragData");
    code = String::regexReplace(
      code, "void\\s+?main\\s*\\(",
      String::printf("%s%s", (hasDrawBuffersExtension ? "" : "out vec4 glFragColor;\n"),
                     "void main("));
  }
  else {
    const auto hasMultiviewExtension = stl_util::contains(defines, "#define MULTIVIEW");
    if (hasMultiviewExtension) {
      return String::printf(
        "#extension GL_OVR_multiview2 : require\nlayout (num_views = 2) in;\n%s", code.c_str());
    }
  }

  return code;
}

} // end of namespace BABYLON
