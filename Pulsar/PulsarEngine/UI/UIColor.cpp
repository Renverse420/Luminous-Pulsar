#include <kamek.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace UI {

static const u8 hudR = 48;
static const u8 hudG = 170;
static const u8 hudB = 170;

static const u8 animR = 0; 
static const u8 animG = 0; 
static const u8 animB = 0;

void GetHUDColor(void* self, RGBA16* c0, RGBA16* c1) {
    c0->red = animR;
    c0->green = animG;
    c0->blue = animB;
    c0->alpha = 0xFD;
    c1->red = hudR;
    c1->green = hudG;
    c1->blue = hudB;
    c1->alpha = 0xFD;
}
kmBranch(0x805f03dc, GetHUDColor);
kmBranch(0x805f0440, GetHUDColor);

void GetHUDBaseColor(void* self, RGBA16* c) {
    c->red = 0;
    c->green = 0;
    c->blue = 0;
    c->alpha = 0x46;
}
kmBranch(0x805f04d8, GetHUDBaseColor);

void GetHUDRaceColor(nw4r::lyt::Pane* _this, u32 idx, nw4r::ut::Color color) {
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
}  // namespace UI
}  // namespace Pulsar