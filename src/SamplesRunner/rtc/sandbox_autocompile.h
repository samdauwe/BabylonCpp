#include <SamplesRunner/rtc/suppress_warnings.h>
#include <SamplesRunner/rtc/link_libraries.h>
#include <RuntimeObjectSystem/ObjectInterfacePerModule.h>
#include <SamplesRunner/rtc/iscene_producer.h>
#include <SamplesRunner/rtc/interface_ids.h>

using namespace BABYLON::rtc;

class Sandbox : public TInterface<IID_RENDERABLESCENE_PRODUCER, ISceneProducer>
{
public:
  std::shared_ptr<BABYLON::IRenderableScene> MakeScene() override;
};
