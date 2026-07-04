#include <kamek.hpp>
#include <IO/IO.hpp>
#include <PulsarSystem.hpp>
#include <MarioKartWii/RKSYS/RKSYSMgr.hpp>

namespace Pulsar {
namespace VanillaSDSave {

static const u32 MAGIC = 'MVR1';
static const u16 VERSION = 1;
static const u32 MAX_LICENSES = 4;
static const u32 MAX_PROFILES = 100;

struct PackedHeader {
    u32 magic;
    u16 version;
    u16 count;
};

struct PackedEntry {
    s32 profileId;
    u16 vr;
    u16 br;
    u32 flags;
};

static PackedEntry sProfiles[MAX_PROFILES] = {};
static bool sLoaded = false;
static char sPath[IOS::ipcMaxPath] __attribute__((aligned(32))) = {};

struct LicenseBackup {
    u16 originalVr;
    u16 originalBr;
    bool hasOriginal;
};
static LicenseBackup sBackups[MAX_LICENSES] = {};

// --- DYNAMIC PATH LOGIC ---
static const char* GetPath() {
    if (sPath[0] == '\0') {
        IO* io = IO::sInstance;
        // If we are on a real Wii (Riivolution), save to the SD save folder.
        if (io && io->type == IOType_RIIVO) {
            snprintf(sPath, IOS::ipcMaxPath, "/riivolution/save/VanillaVR.pul");
        } 
        // If we are on Dolphin, your NANDIO::GetCorrectPath prepends /shared2/Pulsar automatically.
        else {
            snprintf(sPath, IOS::ipcMaxPath, "/VanillaVR.pul");
        }
    }
    return sPath;
}

static void Load() {
    if (sLoaded) return;
    sLoaded = true;

    IO* io = IO::sInstance;
    const char* path = GetPath();
    if (!io || !path || !io->OpenFile(path, FILE_MODE_READ)) return;

    union { PackedHeader h; u8 pad[32]; } hBuf __attribute__((aligned(32))) = {};
    if (io->Read(sizeof(PackedHeader), &hBuf.h) != sizeof(PackedHeader)) {
        io->Close();
        return;
    }
    if (hBuf.h.magic != MAGIC || hBuf.h.version != VERSION) {
        io->Close();
        return;
    }

    u16 count = (hBuf.h.count < MAX_PROFILES) ? hBuf.h.count : MAX_PROFILES;
    for (u16 i = 0; i < count; ++i) {
        union { PackedEntry e; u8 pad[32]; } eBuf __attribute__((aligned(32))) = {};
        if (io->Read(sizeof(PackedEntry), &eBuf.e) != (s32)sizeof(PackedEntry)) break;
        if (eBuf.e.flags & 1) sProfiles[i] = eBuf.e;
    }
    io->Close();
}

static void Save() {
    IO* io = IO::sInstance;
    const char* path = GetPath();
    if (!io || !path) return;

    struct {
        PackedHeader h;
        PackedEntry e[MAX_PROFILES];
        u8 pad[32];
    } file __attribute__((aligned(32))) = {};
    
    file.h.magic = MAGIC;
    file.h.version = VERSION;
    file.h.count = MAX_PROFILES;

    for (u32 i = 0; i < MAX_PROFILES; ++i) file.e[i] = sProfiles[i];

    if (!io->OpenFile(path, FILE_MODE_WRITE)) {
        io->CreateAndOpen(path, FILE_MODE_WRITE);
    }
    io->Overwrite(sizeof(file), &file);
    io->Close();
}

static PackedEntry* GetOrCreateProfile(s32 profileId) {
    if (profileId <= 0) return nullptr;
    for (u32 i = 0; i < MAX_PROFILES; ++i) {
        if (sProfiles[i].flags == 1 && sProfiles[i].profileId == profileId) return &sProfiles[i];
    }
    for (u32 i = 0; i < MAX_PROFILES; ++i) {
        if (sProfiles[i].flags == 0) {
            sProfiles[i].profileId = profileId;
            return &sProfiles[i];
        }
    }
    return nullptr;
}

static void ApplyToLicense(u32 idx, RKSYS::LicenseMgr& lic) {
    Load();
    if (idx >= MAX_LICENSES) return;
    sBackups[idx].originalVr = lic.vr.points;
    sBackups[idx].originalBr = lic.br.points;
    sBackups[idx].hasOriginal = true;

    s32 profileId = lic.dwcAccUserData.gsProfileId;
    PackedEntry* e = GetOrCreateProfile(profileId);
    if (!e) return;

    if (e->flags == 0) {
        e->vr = lic.vr.points;
        e->br = lic.br.points;
        e->flags = 1;
        Save();
    } else {
        lic.vr.points = e->vr;
        lic.br.points = e->br;
    }
}

static void StoreFromLicense(u32 idx, RKSYS::LicenseMgr& lic) {
    Load();
    if (idx >= MAX_LICENSES) return;
    if (!sBackups[idx].hasOriginal) {
        sBackups[idx].originalVr = lic.vr.points;
        sBackups[idx].originalBr = lic.br.points;
        sBackups[idx].hasOriginal = true;
    }
    s32 profileId = lic.dwcAccUserData.gsProfileId;
    PackedEntry* e = GetOrCreateProfile(profileId);
    if (!e) return;
    e->vr = lic.vr.points;
    e->br = lic.br.points;
    e->flags = 1;
    Save();
}

extern "C" int SaveManager_ReadLicenseHook() {
    RKSYS::Mgr* mgr = RKSYS::Mgr::sInstance;
    if (!mgr) return 1;
    RKSYS::LicenseMgr* lic = nullptr;
    asm("mr %0, r31" : "=r"(lic));
    if (!lic) return 1;
    u32 base = reinterpret_cast<u32>(&mgr->licenses[0]);
    u32 addr = reinterpret_cast<u32>(lic);
    if (addr >= base) {
        u32 idx = (addr - base) / sizeof(RKSYS::LicenseMgr);
        if (idx < MAX_LICENSES) ApplyToLicense(idx, *lic);
    }
    return 1;
}

extern "C" void SaveManager_WriteLicenseHook(RKSYS::Binary* raw, u32 idx) {
    RKSYS::Mgr* mgr = RKSYS::Mgr::sInstance;
    if (!mgr || idx >= MAX_LICENSES) return;
    StoreFromLicense(idx, mgr->licenses[idx]);
    if (raw && sBackups[idx].hasOriginal) {
        raw->core.licenses[idx].vr = sBackups[idx].originalVr;
        raw->core.licenses[idx].br = sBackups[idx].originalBr;
    }
}

kmCall(0x805455a8, SaveManager_ReadLicenseHook);
kmCall(0x80546f9c, SaveManager_WriteLicenseHook);

} // namespace VanillaSDSave
} // namespace Pulsar