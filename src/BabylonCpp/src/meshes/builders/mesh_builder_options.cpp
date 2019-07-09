#include <babylon/meshes/builders/mesh_builder_options.h>

namespace BABYLON {

BoxOptions::BoxOptions()
    : size{std::nullopt}
    , width{std::nullopt}
    , height{std::nullopt}
    , depth{std::nullopt}
    , faceUV{{Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
              Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f),
              Vector4(0.f, 0.f, 1.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f)}}
    , faceColors{{Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
                  Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f),
                  Color4(1.f, 1.f, 1.f, 1.f), Color4(1.f, 1.f, 1.f, 1.f)}}
    , sideOrientation{std::nullopt}
    , frontUVs{std::nullopt}
    , backUVs{std::nullopt}
    , wrap{std::nullopt}
    , topBaseAt{std::nullopt}
    , bottomBaseAt{std::nullopt}
    , updatable{std::nullopt}
{
}

BoxOptions::~BoxOptions()
{
}

} // end of namespace BABYLON
