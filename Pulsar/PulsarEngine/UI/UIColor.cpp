#include <kamek.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace UI {
/*
static u8 hudR = 255;
static u8 hudG = 255;
static u8 hudB = 255;

static const u8 hudColors[18][3] = {
    {255, 255, 255},  // White
    {60, 60, 60},  // Black
    {208, 1, 1},  // Red
    {255, 141, 0},  // Orange
    {233, 212, 0},  // Yellow
    {7, 255, 0},  // Green
    {8, 39, 245},  // Blue
    {144, 0, 255},  // Purple
    {255, 201, 237},  // Pink
    {255, 0, 255},  // Magenta
    {36, 224, 255},  // Cyan
    {0, 128, 128},  // Teal
    {212, 175, 55}, // Gold
    {154, 123, 77}, // Brown
    {185, 242, 255}, // Diamond
    {80, 200, 120}, // Emerald
    {202, 155, 255}, // Lavender
    {64, 128, 128} // OptPack
};

void UpdateHUDColor() {
    u8 setting = Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_MENU2, SETTINGMENU2_SCROLL_COLOR);
    if (setting >= 18) setting = 0;
    hudR = hudColors[setting][0];
    hudG = hudColors[setting][1];
    hudB = hudColors[setting][2];
}

void GetHUDColor(void* self, RGBA16* c0, RGBA16* c1) {
    UpdateHUDColor();
    c0->red = hudR;
    c0->green = hudG;
    c0->blue = hudB;
    c0->alpha = 0xFD;
    c1->red = hudR;
    c1->green = hudG;
    c1->blue = hudB;
    c1->alpha = 0xFD;
}
kmBranch(0x805f03dc, GetHUDColor);
kmBranch(0x805f0440, GetHUDColor);

void GetHUDBaseColor(void* self, RGBA16* c) {
    UpdateHUDColor();
    c->red = 0;
    c->green = 0;
    c->blue = 0;
    c->alpha = 0x46;
}
kmBranch(0x805f04d8, GetHUDBaseColor);

void GetHUDRaceColor(nw4r::lyt::Pane* _this, u32 idx, nw4r::ut::Color& color) {
    UpdateHUDColor();
    if (idx < 2) {
        color.r = hudR;
        color.g = hudG;
        color.b = hudB;
        color.a = 0xFD;
    } else {
        color.r = hudR > 20 ? hudR - 20 : 0;
        color.g = hudG > 20 ? hudG - 20 : 0;
        color.b = hudB > 20 ? hudB - 20 : 0;
        color.a = 0xFD;
    }
    _this->SetVtxColor(idx, color);
}
kmCall(0x807ec1dc, GetHUDRaceColor);
*/
}  // namespace UI
}  // namespace Pulsar