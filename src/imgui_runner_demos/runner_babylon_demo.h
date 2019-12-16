#pragma once
#include <memory>
#include <imgui_utils/imgui_runner/abstract_runner.h>

void ShowDemo(std::unique_ptr<ImGui::ImGuiRunner::AbstractRunner> runner);
