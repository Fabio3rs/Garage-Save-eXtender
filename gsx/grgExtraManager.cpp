#define _CRT_SECURE_NO_WARNINGS
#include "grgExtraManager.h"
#include "CStoredCar.h"
#include <cstdint>
#include <injector/calling.hpp>

auto grgExtraManager::OnRestoreCallback() -> grgExtraManager::OnRestoreType & {
    static OnRestoreType cb;
    return cb;
}

auto grgExtraManager::OnRestoreBefCallback()
    -> grgExtraManager::OnRestoreBefType & {
    static OnRestoreBefType cb;
    return cb;
}

auto grgExtraManager::OnSaveCallback() -> grgExtraManager::OnSaveType & {
    static OnSaveType cb;
    return cb;
}

extern "C" {
injector::auto_pointer original_fun, funSave;
CVehicle *using_vehicle = nullptr;
CStoredCar *using_storedCar = nullptr;

void internalWrapperRestore() { grgExtraManager::internalWrapperRestore(); }
void internalWrapperRestoreBef() {
    grgExtraManager::internalWrapperRestoreBef();
}
void internalWrapperSave() { grgExtraManager::internalWrapperSave(); }
void *funRet = nullptr;
}

static void __declspec(naked) restoreHook() {
    __asm__(R"asm(
		mov %ecx, _using_vehicle
		mov %edi, _using_storedCar

		pusha
		pushf

		call _internalWrapperRestore

		popa
		popf

		jmp _original_fun
	)asm");
}

namespace inject {
//
template <uintptr_t V> struct wrapper {
    static auto retaddr() -> void *& {
        static void *addr;
        return addr;
    }

    static void __declspec(naked) restoreHookBeforeSpawn() {
        __asm__ volatile(R"asm(
		mov %ecx, _using_vehicle
		mov %edi, _using_storedCar

		pusha
		pushf

		call _internalWrapperRestore
        )asm");

        __asm__ volatile(R"asm(
            call *%0
            movl 0x0(%%eax), %%eax
        )asm"
                         : "=a"(funRet)
                         : "a"(retaddr));

        __asm__ volatile(R"asm(
            popa
            popf

            jmp *(_funRet)
        )asm");
    }
};

template <uintptr_t T> static void MakeCALL() {
    using E = wrapper<T>;
    E::retaddr() = injector::MakeCALL(T, E::restoreHookBeforeSpawn).get();
}
} // namespace inject

static void __declspec(naked) saveHook() {
    __asm__(R"asm(
		mov %ecx, _using_vehicle
		mov %edi, _using_storedCar

		pusha
		pushf

		call _internalWrapperRestore

		popa
		popf

		jmp _original_fun
	)asm");
}

void grgExtraManager::internalWrapperRestore() {
    grg().veh = using_vehicle;

    auto &restoreFun = OnRestoreCallback();

    if (restoreFun) {
        restoreFun(using_vehicle, using_storedCar);
    }
}

void grgExtraManager::internalWrapperRestoreBef() {
    grg().veh = using_vehicle;

    OnRestoreBefType &restoreFunBef = OnRestoreBefCallback();

    if (restoreFunBef) {
        restoreFunBef(using_vehicle, using_storedCar);
    }
}

void grgExtraManager::internalWrapperSave() {
    grg().veh = using_vehicle;

    auto &saveFun = OnSaveCallback();

    if (saveFun) {
        saveFun(using_vehicle, using_storedCar);
    }
}

auto grgExtraManager::grg() -> grgExtraManager & {
    static grgExtraManager obj;
    return obj;
}

grgExtraManager::grgExtraManager() : veh(nullptr) {
    inject::MakeCALL<0x004480D0>();
    inject::MakeCALL<0x00447F2C>();
    inject::MakeCALL<0x00447F60>();
    inject::MakeCALL<0x00447F94>();
    inject::MakeCALL<0x00447FC8>();
    inject::MakeCALL<0x00447FF8>();
    inject::MakeCALL<0x0044802F>();
    inject::MakeCALL<0x00448074>();
    inject::MakeCALL<0x004480A1>();

    original_fun = injector::MakeCALL(0x0044828D, restoreHook).get();
    // funBef = injector::MakeCALL(0x00447E63, restoreHookBeforeSpawn).get();
    funSave = injector::MakeCALL(0x004498E4, saveHook).get();
}
