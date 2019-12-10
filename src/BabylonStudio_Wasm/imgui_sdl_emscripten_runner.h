#ifndef BABYLONCPP_IMGUI_SDL_EMSCRIPTEN_RUNNER_H
#define BABYLONCPP_IMGUI_SDL_EMSCRIPTEN_RUNNER_H

#include <functional>

using GuiFunction = std::function<void()>;

void run_app(GuiFunction f);

#endif // BABYLONCPP_IMGUI_SDL_EMSCRIPTEN_RUNNER_H
