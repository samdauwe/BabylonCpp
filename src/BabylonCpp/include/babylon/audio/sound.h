#ifndef BABYLON_AUDIO_SOUND_H
#define BABYLON_AUDIO_SOUND_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Sound {

public:
    void play(){}
    void stop(){}

public:
    std::string name;

}; // end of class Sound

} // end of namespace BABYLON

#endif // end of BABYLON_AUDIO_SOUND_H
