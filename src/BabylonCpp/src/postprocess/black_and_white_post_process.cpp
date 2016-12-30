#include <babylon/postprocess/black_and_white_post_process.h>

namespace BABYLON {

BlackAndWhitePostProcess::BlackAndWhitePostProcess(const std::string& _name,
                                                   float ratio, Camera* camera,
                                                   unsigned int samplingMode,
                                                   Engine* engine,
                                                   bool reusable)
    : PostProcess{_name,  "blackAndWhite", {},     {},      ratio,
                  camera, samplingMode,    engine, reusable}
{
}

BlackAndWhitePostProcess::~BlackAndWhitePostProcess()
{
}

} // end of namespace BABYLON
