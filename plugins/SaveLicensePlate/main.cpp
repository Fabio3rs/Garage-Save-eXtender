//#define _CRT_SECURE_NO_WARNINGS
#include "../../injector/saving.hpp"
#include "CStoredCar.h"
#include "GSXAPI.h"
#include <fstream>
#include <injector/calling.hpp>
#include <plugin_sa/game_sa/CVehicle.h>
#include <plugin_sa/game_sa/RenderWare.h>
#include <string>
#include <windows.h>

namespace {
auto getVehicleModelInfoByID =
    injector::cstd<CVehicleModelInfo *(int id)>::call<0x00403DA0>;
auto CustomCarPlate_TextureCreate =
    injector::thiscall<bool(CVehicle *, CVehicleModelInfo *)>::call<0x006D10E0>;
auto localRpMaterialSetTexture =
    injector::cstd<RpMaterial *(RpMaterial *, RwTexture *)>::call<0x0074DBC0>;
auto licensePlateTextureCreate0 = injector::cstd<RwTexture *(
    RpMaterial *material, const char *a2, char a3)>::call<0x006FE020>;
auto generateLicenseplateMaterial = injector::cstd<RpMaterial *(
    RpClump *clump, const char *pData, char a3)>::call<0x006FE0F0>;
} // namespace

std::fstream saveLicensePlateLog("saveLicensePlate.log",
                                 std::ios::out | std::ios::trunc);

extern "C" {
CVehicle *tempVeh = nullptr;
}

void callback(const GSX::externalCallbackStructure *test) {
    using namespace GSX;

    if (test->veh == nullptr) {
        return;
    }

    tempVeh = test->veh;
    saveLicensePlateLog << std::to_string((uintptr_t)test->veh) << " status   "
                        << test->status << std::endl;

    switch (test->status) {
    case GSX::LOAD_CAR: {
        const char *plateText =
            (const char *)getSavedData(test->veh, "carplate");

        if (plateText != nullptr) {
            CVehicleModelInfo *info =
                getVehicleModelInfoByID(test->veh->m_nModelIndex);

            strncpy(info->m_szPlateText, plateText, 8u);

            saveLicensePlateLog << "Before CustomCarPlate_TextureCreate"
                                << std::endl;

            if (CustomCarPlate_TextureCreate(test->veh, info)) {
                char testPlate[9] = {0};

                strncpy(testPlate, plateText, 8u);
                testPlate[8] = 0;
                saveLicensePlateLog << "Plate type " << (int)info->m_nPlateType
                                    << std::endl;

                saveLicensePlateLog
                    << std::to_string((uintptr_t)test->veh)
                    << " GTA SA function CustomCarPlate_TextureCreate: OK  "
                    << testPlate << std::endl;
            } else {
                saveLicensePlateLog << std::to_string((uintptr_t)test->veh)
                                    << " GTA SA function "
                                       "CustomCarPlate_TextureCreate: error"
                                    << std::endl;
            }
        } else {
            saveLicensePlateLog << std::to_string((uintptr_t)test->veh)
                                << " plateText is nullptr " << std::endl;
        }

        break;
    }

    case GSX::SAVE_CAR: {
        if (test->veh->m_pCustomCarPlate) {
            setDataToSaveLater(test->veh, "carplate", 8,
                               test->veh->m_pCustomCarPlate->name, true);

            char testPlate[9] = {0};

            strncpy(testPlate, test->veh->m_pCustomCarPlate->name, 8u);
            testPlate[8] = 0;

            saveLicensePlateLog << std::to_string((uintptr_t)test->veh)
                                << " saving license plate " << testPlate
                                << std::endl;
        } else {
            saveLicensePlateLog << std::to_string((uintptr_t)test->veh)
                                << " veh->m_pCustomCarPlate is nullptr"
                                << std::endl;
        }
        break;
    }

    case GSX::LOAD_CAR_BEFORE_SPAWN: {
        CVehicleModelInfo *info =
            getVehicleModelInfoByID(test->gameStoredData->model);

        int *idptr = (int *)GSX::getSavedData(test->veh, "licplateID");

        if (idptr != nullptr) {
            info->m_nPlateType = *idptr;
        }
    } break;

    default:
        saveLicensePlateLog << "GSX unknow status" << std::endl;
        break;
    }
}

uint16_t &CWeather__CurrLevel = *(uint16_t *)0x00C81314;
RwTexture **plateTexts = (RwTexture **)0x00C3EF60;

extern "C" {
injector::auto_pointer hookPlateDrawOriginal;
}

void __declspec(naked) hookPlateDraw() {
    asm(
        R"asm(
            mov %ecx, _tempVeh

		    jmp *(_hookPlateDrawOriginal)
        )asm");
}

// 0x006FDE50
RpMaterial *__cdecl platetext(RpMaterial *material, char a2) {
    int v2 = a2;

    if (a2 == -1) {
        if (CWeather__CurrLevel == 1)
            v2 = 2;
        else
            v2 = CWeather__CurrLevel > 2 && CWeather__CurrLevel <= 4;
    }

    int *idptr = (int *)GSX::getSavedData(tempVeh, "licplateID");

    if (idptr != nullptr) {
        // v2 = *idptr;
    } else {
        GSX::pushDirectlyToSavedData(tempVeh, "licplateID", sizeof(v2), &v2);
    }

    localRpMaterialSetTexture(material, plateTexts[v2]);
    return material;
}

void onload(int id) {
    static bool loaded = false;
    saveLicensePlateLog << "onload " << id << std::endl;

    if (!loaded) {
        saveLicensePlateLog << "ASI date/time: " __DATE__ " " __TIME__
                            << std::endl;

        loaded = true;

        addNotifyCallback(callback);

        plateTexts = injector::ReadMemory<RwTexture **>(0x006FDE7B + 3, true);

        injector::MakeJMP(0x006FDE50, platetext);
        hookPlateDrawOriginal =
            injector::MakeCALL(0x006D651C, hookPlateDraw).get();
    }
}

void inject() {
    saveLicensePlateLog << "inject " << std::endl;
    injector::save_manager::on_load(onload);
}

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason,
                    _In_ LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        freopen("conout$", "w", stdout);
        freopen("conout$", "w", stderr);
        freopen("conin$", "r", stdin);
        inject();
    }

    return true;
}
