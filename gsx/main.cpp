
#define _CRT_SECURE_NO_WARNINGS
#ifndef GTASA
#define GTASA
#endif

#include "../injector/saving.hpp"
#include "CLog.h"
#include "CVehicleExtraData.h"
#include "InternalCallbacks.hpp"
#include "grgExtraManager.h"
#include <Events.h>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <string>
#include <windows.h>

using ngamectscptp_hook =
    injector::function_hooker<injector::scoped_call, 5487817, void()>;
using ngamectscptp_hook2 =
    injector::function_hooker<injector::scoped_call, 6393605, void()>;
using ngamectscptp_hook3 =
    injector::function_hooker<injector::scoped_call, 5488269, void()>;

void installClearHooks() {
    injector::make_static_hook<ngamectscptp_hook>(
        [](const ngamectscptp_hook::func_type &func) {
            GSX::InternalCallbacks::staticHookClearData();
            return func();
        });

    injector::make_static_hook<ngamectscptp_hook2>(
        [](const ngamectscptp_hook2::func_type &func) {
            GSX::InternalCallbacks::staticHookClearData();
            return func();
        });

    injector::make_static_hook<ngamectscptp_hook3>(
        [](const ngamectscptp_hook3::func_type &func) {
            GSX::InternalCallbacks::staticHookClearData();
            return func();
        });
}

void vehDtorEvent(CVehicle *veh) {
    CVehicleExtraData::inst().getDataByVehPtr(veh).destroy();
}

void hook() {
    CLog::log() << "void hook() function";

    grgExtraManager::grg();
    grgExtraManager::on_restore(GSX::InternalCallbacks::restore);
    grgExtraManager::on_restore_bef(
        GSX::InternalCallbacks::restoreDataBeforeSpawn);
    grgExtraManager::on_save(GSX::InternalCallbacks::save);

    CVehicleExtraData::inst();
    plugin::Events::vehicleDtorEvent.Add(vehDtorEvent, plugin::PRIORITY_AFTER);

    injector::save_manager::on_load(GSX::InternalCallbacks::onLoadGame);
    injector::save_manager::on_save(GSX::InternalCallbacks::onSaveGame);

    installClearHooks();
}

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason,
                    _In_ LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        hook();
    }

    return true;
}