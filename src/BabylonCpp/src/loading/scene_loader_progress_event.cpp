#include <babylon/loading/scene_loader_progress_event.h>

#include <babylon/loading/progress_event.h>

namespace BABYLON {

SceneLoaderProgressEvent::SceneLoaderProgressEvent(bool lengthComputable,
                                                   size_t loaded, size_t total)
    : lengthComputable{this, &SceneLoaderProgressEvent::get_lengthComputable}
    , loaded{this, &SceneLoaderProgressEvent::get_loaded}
    , total{this, &SceneLoaderProgressEvent::get_total}
    , _lengthComputable{lengthComputable}
    , _loaded{loaded}
    , _total{total}
{
}

SceneLoaderProgressEvent::SceneLoaderProgressEvent(
  const SceneLoaderProgressEvent& other)
    : lengthComputable{this, &SceneLoaderProgressEvent::get_lengthComputable}
    , loaded{this, &SceneLoaderProgressEvent::get_loaded}
    , total{this, &SceneLoaderProgressEvent::get_total}
    , _lengthComputable{other.lengthComputable}
    , _loaded{other.loaded}
    , _total{other.total}
{
}

SceneLoaderProgressEvent::SceneLoaderProgressEvent(
  SceneLoaderProgressEvent&& other)
    : lengthComputable{this, &SceneLoaderProgressEvent::get_lengthComputable}
    , loaded{this, &SceneLoaderProgressEvent::get_loaded}
    , total{this, &SceneLoaderProgressEvent::get_total}
    , _lengthComputable{std::move(other.lengthComputable)}
    , _loaded{std::move(other.loaded)}
    , _total{std::move(other.total)}
{
}

SceneLoaderProgressEvent& SceneLoaderProgressEvent::
operator=(const SceneLoaderProgressEvent& other)
{
  if (&other != this) {
    _lengthComputable = other.lengthComputable;
    _loaded           = other.loaded;
    _total            = other.total;
  }

  return *this;
}

SceneLoaderProgressEvent& SceneLoaderProgressEvent::
operator=(SceneLoaderProgressEvent&& other)
{
  if (&other != this) {
    _lengthComputable = std::move(other.lengthComputable);
    _loaded           = std::move(other.loaded);
    _total            = std::move(other.total);
  }

  return *this;
}

SceneLoaderProgressEvent::~SceneLoaderProgressEvent() = default;

bool SceneLoaderProgressEvent::get_lengthComputable() const
{
  return _lengthComputable;
}

size_t SceneLoaderProgressEvent::get_loaded() const
{
  return _loaded;
}

size_t SceneLoaderProgressEvent::get_total() const
{
  return _total;
}

SceneLoaderProgressEvent
SceneLoaderProgressEvent::FromProgressEvent(const ProgressEvent& event)
{
  return SceneLoaderProgressEvent(event.lengthComputable, event.loaded,
                                  event.total);
}

} // end of namespace BABYLON
