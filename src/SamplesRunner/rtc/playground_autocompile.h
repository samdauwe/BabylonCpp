#include <SamplesRunner/rtc/suppress_warnings.h>
#include <SamplesRunner/rtc/link_libraries.h>
#include <RuntimeObjectSystem/ObjectInterfacePerModule.h>
#include <SamplesRunner/rtc/iscene_producer.h>
#include <SamplesRunner/rtc/interface_ids.h>

class Playground : public TInterface<BABYLON::rtc::IID_RENDERABLESCENE_PRODUCER, BABYLON::rtc::ISceneProducer>
{
public:
  std::shared_ptr<BABYLON::IRenderableSceneWithHud> MakeScene() override;
};
