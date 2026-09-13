#include "menu.h"
#include "config/config.h"
#include <imgui.h>

namespace menu {

void apply_theme() {
    auto& s = ImGui::GetStyle();
    s.WindowRounding = 12.f; s.FrameRounding = 8.f; s.GrabRounding = 8.f;
    s.WindowBorderSize = 0.f; s.FrameBorderSize = 0.f;
    s.WindowPadding = { 14.f, 14.f };
    s.ItemSpacing   = { 10.f, 10.f };
    s.TouchExtraPadding = { 8.f, 8.f };

    auto* c = s.Colors;
    c[ImGuiCol_WindowBg]       = { 0.035f, 0.047f, 0.090f, 0.96f };
    c[ImGuiCol_ChildBg]        = { 0.047f, 0.062f, 0.118f, 0.80f };
    c[ImGuiCol_Border]         = { 0.18f, 0.32f, 0.60f, 0.65f };
    c[ImGuiCol_FrameBg]        = { 0.070f, 0.094f, 0.176f, 1.00f };
    c[ImGuiCol_FrameBgHovered] = { 0.110f, 0.176f, 0.310f, 1.00f };
    c[ImGuiCol_FrameBgActive]  = { 0.180f, 0.290f, 0.500f, 1.00f };
    c[ImGuiCol_CheckMark]      = { 0.29f, 0.62f, 1.00f, 1.00f };
    c[ImGuiCol_SliderGrab]     = { 0.29f, 0.62f, 1.00f, 1.00f };
    c[ImGuiCol_SliderGrabActive] = { 0.45f, 0.75f, 1.00f, 1.00f };
    c[ImGuiCol_Button]         = { 0.090f, 0.140f, 0.260f, 1.00f };
    c[ImGuiCol_ButtonHovered]  = { 0.180f, 0.290f, 0.500f, 1.00f };
    c[ImGuiCol_ButtonActive]   = { 0.29f, 0.62f, 1.00f, 1.00f };
    c[ImGuiCol_Header]         = { 0.090f, 0.140f, 0.260f, 1.00f };
    c[ImGuiCol_HeaderHovered]  = { 0.180f, 0.290f, 0.500f, 1.00f };
    c[ImGuiCol_HeaderActive]   = { 0.29f, 0.62f, 1.00f, 1.00f };
    c[ImGuiCol_Separator]      = { 0.18f, 0.32f, 0.60f, 0.40f };
    c[ImGuiCol_Tab]            = { 0.070f, 0.094f, 0.176f, 1.00f };
    c[ImGuiCol_TabHovered]     = { 0.29f, 0.62f, 1.00f, 0.80f };
    c[ImGuiCol_TabActive]      = { 0.29f, 0.62f, 1.00f, 1.00f };
    c[ImGuiCol_TabUnfocused]   = { 0.070f, 0.094f, 0.176f, 0.60f };
    c[ImGuiCol_TabUnfocusedActive] = { 0.180f, 0.290f, 0.500f, 0.80f };
    c[ImGuiCol_Text]           = { 0.90f, 0.94f, 1.00f, 1.00f };
    c[ImGuiCol_TextDisabled]   = { 0.45f, 0.55f, 0.75f, 1.00f };
}

static void tab_esp() {
    auto& e = cfg::esp;
    ImGui::Checkbox("Enable ESP", &e.enabled); ImGui::SameLine();
    ImGui::Checkbox("Team", &e.team_check);
    ImGui::Checkbox("Box", &e.box);       ImGui::SameLine();
    ImGui::Checkbox("3D", &e.box_3d);
    ImGui::Checkbox("Corner", &e.corner); ImGui::SameLine();
    ImGui::Checkbox("Gradient", &e.gradient);
    ImGui::Checkbox("Text", &e.text);     ImGui::SameLine();
    ImGui::Checkbox("Health", &e.health);
    ImGui::Checkbox("Snapline", &e.snapline);
    ImGui::SliderFloat("Max dist", &e.max_dist, 50.f, 1000.f, "%.0f");
    ImGui::SeparatorText("Colors");
    ImGui::ColorEdit4("Box",    cfg::colors.box);
    ImGui::ColorEdit4("Vis",    cfg::colors.box_vis);
    ImGui::ColorEdit4("Corner", cfg::colors.corner);
    ImGui::ColorEdit4("Text",   cfg::colors.text);
    ImGui::ColorEdit4("Grad T", cfg::colors.gradient_top);
    ImGui::ColorEdit4("Grad B", cfg::colors.gradient_bot);
}

static void tab_aim() {
    auto& a = cfg::aimbot;
    ImGui::Checkbox("Enable Aimbot", &a.enabled);
    ImGui::Checkbox("Draw FOV",      &a.draw_fov);
    const char* bones[] = { "Head","Chest","Pelvis","Nearest" };
    ImGui::Combo("Bone", &a.bone, bones, 4);
    ImGui::SliderFloat("FOV",    &a.fov,    1.f, 360.f, "%.0f");
    ImGui::SliderFloat("Smooth", &a.smooth, 1.f,  20.f, "%.1f");
    ImGui::SeparatorText("Checks");
    ImGui::Checkbox("Visible", &a.visible_check);
    ImGui::Checkbox("Fire",    &a.fire_check);
    ImGui::Checkbox("Shoot",   &a.shoot_check);
}

static void tab_chams() {
    auto& c = cfg::chams;
    ImGui::Checkbox("Enable",  &c.enabled);
    ImGui::Checkbox("Glass",   &c.glass);
    ImGui::Checkbox("IgnoreZ", &c.ignore_z);
    ImGui::ColorEdit4("Color", cfg::colors.chams);
}

static void tab_aa() {
    auto& a = cfg::antiaim;
    ImGui::Checkbox("Enable", &a.enabled);
    const char* m[] = { "Random","Spin","Jitter" };
    ImGui::Combo("Mode", &a.mode, m, 3);
    ImGui::SliderFloat("Speed", &a.speed, 1.f, 60.f, "%.0f");
}

static void tab_misc() {
    ImGui::Checkbox("Watermark",    &cfg::misc.watermark);
    ImGui::Checkbox("Stream proof", &cfg::misc.stream_proof);
    ImGui::TextWrapped("Menu toggle: tap top-left corner twice.");
}

void render() {
    ImGui::SetNextWindowSize({ 640.f, 520.f }, ImGuiCond_FirstUseEver);
    ImGui::Begin("Airware Internal", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::TextColored({ 0.29f, 0.62f, 1.f, 1.f }, "Airware Internal");
    ImGui::SameLine();
    ImGui::TextDisabled("| S2 v0.39.4 | arm64");
    ImGui::Separator();
    if (ImGui::BeginTabBar("##tabs")) {
        if (ImGui::BeginTabItem("ESP"))     { tab_esp();   ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Aimbot"))  { tab_aim();   ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Chams"))   { tab_chams(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("AntiAim")) { tab_aa();    ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Misc"))    { tab_misc();  ImGui::EndTabItem(); }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

} // namespace menu
