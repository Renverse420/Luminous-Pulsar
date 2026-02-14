#include <kamek.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/RKNet/Select.hpp>
#include <MarioKartWii/System/random.hpp>
#include <MarioKartWii/Race/Racedata.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <Settings/Settings.hpp>
#include <Network/PulSELECT.hpp>
#include <PulsarSystem.hpp>
#include <Gamemodes/KO/KOMgr.hpp>
#include <MarioKartWii/UI/Ctrl/SheetSelect.hpp>
#include <MarioKartWii/UI/Page/RaceMenu/RaceMenu.hpp>
#include <UI/UI.hpp>
#include <Network/Network.hpp>

namespace Pulsar {
namespace Network {
//No disconnect for being idle (Bully)
//kmWrite32(0x80521408, 0x38000000);
//kmWrite32(0x8053EF6C, 0x38000000);
//kmWrite32(0x8053F0B4, 0x38000000);
//kmWrite32(0x8053F124, 0x38000000);

static bool IsWiFiMenuSection(SectionId id) {
    switch (id) {
        case SECTION_MAIN_MENU_FROM_BOOT:
        case SECTION_MAIN_MENU_FROM_RESET:
        case SECTION_MAIN_MENU_FROM_MENU:
        case SECTION_MAIN_MENU_FROM_NEW_LICENSE:
        case SECTION_MAIN_MENU_FROM_LICENSE:
        case SECTION_MII_SELECT_1:
        case SECTION_MII_SELECT_2:
        case SECTION_LICENSE_SETTINGS_MENU:
        case SECTION_SINGLE_P_FROM_MENU:
        case SECTION_LOCAL_MULTIPLAYER:
        case SECTION_P1_WIFI:
        case SECTION_P1_WIFI_FROM_FIND_FRIEND:
        case SECTION_P2_WIFI:
        case SECTION_P2_WIFI_FROM_FIND_FRIEND:
        case SECTION_WIFI_DISCONNECT_ERROR:
        case SECTION_WIFI_DISCONNECT_GENERAL:
            return true;
        default:
            return false;
    }
}

void ResetTrackBlockingOnRoomEnd() {
    SectionMgr* sectionMgr = SectionMgr::sInstance;
    if (sectionMgr == nullptr || sectionMgr->curSection == nullptr) return;

    const SectionId sectionId = sectionMgr->curSection->sectionId;
    if (!IsWiFiMenuSection(sectionId)) return;

    System* system = System::sInstance;
    if (system == nullptr) return;

    Mgr& netMgr = system->netMgr;
    const u32 blockingCount = system->GetInfo().GetTrackBlocking();

    if (netMgr.lastTracks != nullptr && blockingCount > 0) {
        for (u32 i = 0; i < blockingCount; ++i) {
            netMgr.lastTracks[i] = PULSARID_NONE;
        }
        netMgr.curBlockingArrayIdx = 0;
    }
}
static SectionLoadHook resetTrackBlockingHook(ResetTrackBlockingOnRoomEnd);

static void CalcSectionAfterRace(SectionMgr* sectionMgr, SectionId id) {

    //UI::ChooseNextTrack* choosePage = reinterpret_cast<UI::ExpSection*>(sectionMgr->curSection)->GetPulPage<UI::ChooseNextTrack>();
    const System* system = System::sInstance;
    //if(choosePage != nullptr) id = choosePage->ProcessHAW(id);
    if(id != SECTION_NONE) {
        if(system->IsContext(PULSAR_MODE_KO)) id = system->koMgr->GetSectionAfterKO(id);
        sectionMgr->SetNextSection(id, 0);
        register Pages::WWRaceEndWait* wait;
        asm(mr wait, r31);
        wait->EndStateAnimated(0.0f, 0);
        sectionMgr->RequestSceneChange(0, 0xFF);
    }
}
kmCall(0x8064f5fc, CalcSectionAfterRace);
kmPatchExitPoint(CalcSectionAfterRace, 0x8064f648);

}//namespace Network
}//namespace Pulsar