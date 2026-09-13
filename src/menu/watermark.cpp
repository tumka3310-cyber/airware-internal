#include "watermark.h"
#include "config/config.h"
#include <imgui.h>

namespace watermark {

void render() {
    if (!cfg::misc.watermark) return;

    ImGuiIO& io = ImGui::GetIO();
    const char* text = "Airware Internal

    ImVec2 sz = ImGui::CalcTextSize(text);
    ImVec2 pos { 12.f, 12.f };

    auto* dl = ImGui::GetForegroundDrawList();
    dl->AddRectFilled({ pos.x - 10.f, pos.y - 6.f },
                      { pos.x + sz.x + 10.f, pos.y + sz.y + 6.f },
                      IM_COL32(8, 12, 24, 200), 6.f);
    dl->AddRect      ({ pos.x - 10.f, pos.y - 6.f },
                      { pos.x + sz.x + 10.f, pos.y + sz.y + 6.f },
                      IM_COL32(74, 158, 255, 220), 6.f, 0, 1.2f);
    dl->AddRectFilled({ pos.x - 10.f, pos.y - 6.f },
                      { pos.x - 6.f,  pos.y + sz.y + 6.f },
                      IM_COL32(74, 158, 255, 255), 6.f);
    dl->AddText(pos, IM_COL32(230, 240, 255, 255), text);
}

} // namespace watermark
