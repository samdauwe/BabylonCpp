#include <babylon/engines/webgl/webgl_shader_processor.h>

#include <babylon/engines/thin_engine.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

WebGLShaderProcessor::~WebGLShaderProcessor() = default;

std::string WebGLShaderProcessor::postProcessor(std::string code,
                                                const std::vector<std::string>& /*defines*/,
                                                bool /*isFragment*/, ThinEngine* engine)
{
  // Remove extensions
  if (!engine->getCaps().drawBuffersExtension) {
    // even if enclosed in #if/#endif, IE11 does parse the #extension declaration, so we need to
    // remove it altogether
    const std::string regex("#extension.+GL_EXT_draw_buffers.+(enable|require)");
    code = StringTools::regexReplace(code, regex, "");
  }

  return code;
}

} // end of namespace BABYLON
