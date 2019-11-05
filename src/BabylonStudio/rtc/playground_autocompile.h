#include <BabylonStudio/rtc/suppress_warnings.h>
#include <BabylonStudio/rtc/link_libraries.h>
#include <RuntimeObjectSystem/ObjectInterfacePerModule.h>
#include <BabylonStudio/rtc/iscene_producer.h>
#include <BabylonStudio/rtc/interface_ids.h>

class Playground : public TInterface<BABYLON::rtc::IID_RENDERABLESCENE_PRODUCER, BABYLON::rtc::ISceneProducer>
{
public:
  std::shared_ptr<BABYLON::IRenderableSceneWithHud> MakeScene() override;
};
