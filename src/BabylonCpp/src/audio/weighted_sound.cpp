#include <babylon/audio/weighted_sound.h>

#include <babylon/audio/sound.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/random.h>

namespace BABYLON {

WeightedSound::WeightedSound(bool iLoop, const std::vector<SoundPtr>& sounds,
                             const Float32Array& weights)
    : loop{false}
    , isPlaying{false}
    , isPaused{false}
    , directionalConeInnerAngle{this,
                                &WeightedSound::get_directionalConeInnerAngle,
                                &WeightedSound::set_directionalConeInnerAngle}
    , directionalConeOuterAngle{this,
                                &WeightedSound::get_directionalConeOuterAngle,
                                &WeightedSound::set_directionalConeOuterAngle}
    , volume{this, &WeightedSound::get_volume, &WeightedSound::set_volume}
    , _coneInnerAngle{360.f}
    , _coneOuterAngle{360.f}
    , _volume{1}
    , _currentIndex{std::nullopt}
{
  if (sounds.size() != weights.size()) {
    throw std::runtime_error("sounds length does not equal weights length");
  }

  loop     = iLoop;
  _weights = weights;
  // Normalize the weights
  float weightSum = 0;
  for (const auto weight : weights) {
    weightSum += weight;
  }
  float invWeightSum = weightSum > 0.f ? 1.f / weightSum : 0.f;
  for (size_t i = 0; i < _weights.size(); ++i) {
    _weights[i] *= invWeightSum;
  }
  _sounds = sounds;
  for (const auto& sound : _sounds) {
    sound->onEndedObservable.add(
      [this](Sound* /*sound*/, EventState& /*es*/) { _onended(); });
  }
}

WeightedSound::~WeightedSound()
{
}

float WeightedSound::get_directionalConeInnerAngle() const
{
  return _coneInnerAngle;
}

void WeightedSound::set_directionalConeInnerAngle(float value)
{
  if (!stl_util::almost_equal(value, _coneInnerAngle)) {
    if (_coneOuterAngle < value) {
      BABYLON_LOG_ERROR("WeightedSound",
                        "directionalConeInnerAngle: outer angle of the cone "
                        "must be superior or equal to the inner angle.");
      return;
    }

    _coneInnerAngle = value;
    for (const auto& sound : _sounds) {
      sound->directionalConeInnerAngle = value;
    }
  }
}

float WeightedSound::get_directionalConeOuterAngle() const
{
  return _coneOuterAngle;
}

void WeightedSound::set_directionalConeOuterAngle(float value)
{
  if (!stl_util::almost_equal(value, _coneOuterAngle)) {
    if (value < _coneInnerAngle) {
      BABYLON_LOG_ERROR("WeightedSound",
                        "directionalConeOuterAngle: outer angle of the cone "
                        "must be superior or equal to the inner angle.");
      return;
    }

    _coneOuterAngle = value;
    for (const auto& sound : _sounds) {
      sound->directionalConeOuterAngle = value;
    }
  }
}

unsigned int WeightedSound::get_volume() const
{
  return _volume;
}

void WeightedSound::set_volume(unsigned int value)
{
  if (value != _volume) {
    for (const auto& sound : _sounds) {
      sound->setVolume(value);
    }
  }
}

void WeightedSound::_onended()
{
  if (_currentIndex.has_value() && *_currentIndex < _sounds.size()) {
    _sounds[*_currentIndex]->autoplay = false;
  }
  if (loop && isPlaying) {
    play();
  }
  else {
    isPlaying = false;
  }
}

void WeightedSound::pause()
{
  isPaused = false;
  if (_currentIndex.has_value() && *_currentIndex < _sounds.size()) {
    _sounds[*_currentIndex]->pause();
  }
}

void WeightedSound::stop()
{
  isPlaying = false;
  if (_currentIndex.has_value() && *_currentIndex < _sounds.size()) {
    _sounds[*_currentIndex]->stop();
  }
}

void WeightedSound::play(const std::optional<size_t>& startOffset)
{
  if (!isPaused) {
    stop();
    auto randomValue = Math::random();
    float total      = 0.f;
    for (size_t i = 0; i < _weights.size(); ++i) {
      total += _weights[i];
      if (randomValue <= total) {
        _currentIndex = i;
        break;
      }
    }
  }
  if (_currentIndex.has_value()) {
    const auto& sound = _sounds[*_currentIndex];
    if (sound->isReady()) {
      sound->play(0, isPaused ? std::nullopt : startOffset);
    }
    else {
      sound->autoplay = true;
    }
    isPlaying = true;
    isPaused  = false;
  }
}

} // end of namespace BABYLON
