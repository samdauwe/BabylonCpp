Docking:
  - Inspector: only one dock
      => use tabs, ou bien faire un DockSpace interne
  - SamplesBrowser a gauche
  - 3D à droite
  - Playground: suppr 3D... 

if (ImGui::BeginTabBar("blah"))
{
    if (ImGui::BeginTabItem("Video"))
    {
        ImGui::Text("Blah blah");
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Audio"))
    {
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Controls"))
    {
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
}