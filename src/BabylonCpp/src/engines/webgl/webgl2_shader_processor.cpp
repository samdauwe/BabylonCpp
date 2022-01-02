#include <babylon/engines/webgl/webgl2_shader_processor.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

WebGL2ShaderProcessor::~WebGL2ShaderProcessor() = default;

std::string WebGL2ShaderProcessor::attributeProcessor(const std::string& attribute)
{
  return StringTools::replace(attribute, "attribute", "in");
}

std::string WebGL2ShaderProcessor::varyingProcessor(const std::string& varying, bool isFragment)
{
  return StringTools::replace(varying, "varying", isFragment ? "in" : "out");
}

std::string WebGL2ShaderProcessor::postProcessor(std::string code,
                                                 const std::vector<std::string>& defines,
                                                 bool isFragment, ThinEngine* /*engine*/)
{
  const auto hasDrawBuffersExtension
    = StringTools::contains(code, "#extension.+GL_EXT_draw_buffers.+require");

  // Remove extensions
  const std::string regex(
    "#extension.+(GL_OVR_multiview2|GL_OES_standard_derivatives|GL_EXT_shader_texture_lod|GL_EXT_"
    "frag_depth|GL_EXT_draw_buffers).+(enable|require)");
  code = StringTools::regexReplace(code, regex, "");

  // Replace instructions
  code = StringTools::regexReplace(code, "texture2D\\s*\\(", "texture(");
  if (isFragment) {
    code = StringTools::regexReplace(code, "texture2DLodEXT\\s*\\(", "textureLod(");
    code = StringTools::regexReplace(code, "textureCubeLodEXT\\s*\\(", "textureLod(");
    code = StringTools::regexReplace(code, "textureCube\\s*\\(", "texture(");
    code = StringTools::regexReplace(code, "gl_FragDepthEXT", "gl_FragDepth");
    code = StringTools::regexReplace(code, "gl_FragColor", "glFragColor");
    code = StringTools::regexReplace(code, "gl_FragData", "glFragData");
    code = StringTools::regexReplace(
      code, R"(void\s+?main\s*\()",
      StringTools::printf("%s%s", (hasDrawBuffersExtension ? "" : "out vec4 glFragColor;\n"),
                          "void main("));
  }
  else {
    const auto hasMultiviewExtension = stl_util::contains(defines, "#define MULTIVIEW");
    if (hasMultiviewExtension) {
      return StringTools::printf(
        "#extension GL_OVR_multiview2 : require\nlayout (num_views = 2) in;\n%s", code.c_str());
    }
  }

  return code;
}

} // end of namespace BABYLON
