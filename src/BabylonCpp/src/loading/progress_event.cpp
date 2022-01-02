#include <babylon/loading/progress_event.h>

namespace BABYLON {

ProgressEvent::ProgressEvent(const std::string& type, bool lengthComputable,
                             size_t loaded, size_t total)
    : type{this, &ProgressEvent::get_type}
    , lengthComputable{this, &ProgressEvent::get_lengthComputable}
    , loaded{this, &ProgressEvent::get_loaded}
    , total{this, &ProgressEvent::get_total}
    , _type{type}
    , _lengthComputable{lengthComputable}
    , _loaded{loaded}
    , _total{total}
{
}

ProgressEvent::ProgressEvent(const ProgressEvent& other)
    : type{this, &ProgressEvent::get_type}
    , lengthComputable{this, &ProgressEvent::get_lengthComputable}
    , loaded{this, &ProgressEvent::get_loaded}
    , total{this, &ProgressEvent::get_total}
    , _type{other.type}
    , _lengthComputable{other.lengthComputable}
    , _loaded{other.loaded}
    , _total{other.total}
{
}

ProgressEvent::ProgressEvent(ProgressEvent&& other)
    : type{this, &ProgressEvent::get_type}
    , lengthComputable{this, &ProgressEvent::get_lengthComputable}
    , loaded{this, &ProgressEvent::get_loaded}
    , total{this, &ProgressEvent::get_total}
    , _type{std::move(other.type)}
    , _lengthComputable{std::move(other.lengthComputable)}
    , _loaded{std::move(other.loaded)}
    , _total{std::move(other.total)}
{
}

ProgressEvent& ProgressEvent::operator=(const ProgressEvent& other)
{
  if (&other != this) {
    _type             = other.type;
    _lengthComputable = other.lengthComputable;
    _loaded           = other.loaded;
    _total            = other.total;
  }

  return *this;
}

ProgressEvent& ProgressEvent::operator=(ProgressEvent&& other)
{
  if (&other != this) {
    _type             = std::move(other.type);
    _lengthComputable = std::move(other.lengthComputable);
    _loaded           = std::move(other.loaded);
    _total            = std::move(other.total);
  }

  return *this;
}

ProgressEvent::~ProgressEvent() = default;

std::string ProgressEvent::get_type() const
{
  return _type;
}

bool ProgressEvent::get_lengthComputable() const
{
  return _lengthComputable;
}

size_t ProgressEvent::get_loaded() const
{
  return _loaded;
}

size_t ProgressEvent::get_total() const
{
  return _total;
}

} // end of namespace BABYLON
