#pragma once
#ifndef GTASA_GRGEXTRAITEMS
#define GTASA_GRGEXTRAITEMS
#include <functional>

class CVehicle;
class CStoredCar;

class grgExtraManager {
    CVehicle *veh;

  public:
    using OnRestoreType = std::function<void(CVehicle *, CStoredCar *)>;
    using OnRestoreBefType = std::function<void(CVehicle *, CStoredCar *)>;
    using OnSaveType = std::function<void(CVehicle *, CStoredCar *)>;

    static void internalWrapperRestore();
    static void internalWrapperRestoreBef();
    static void internalWrapperSave();

    static void on_restore(const OnRestoreType &fn) {
        grg();
        OnRestoreCallback() = fn;
    }

    static void on_restore_bef(const OnRestoreBefType &fn) {
        grg();
        OnRestoreBefCallback() = fn;
    }

    static void on_save(const OnSaveType &fn) {
        grg();
        OnSaveCallback() = fn;
    }

    auto getVeh() -> CVehicle * { return veh; }

    static auto grg() -> grgExtraManager &;

  protected:
    grgExtraManager();

    static auto OnRestoreCallback() -> OnRestoreType &;
    static auto OnRestoreBefCallback() -> OnRestoreBefType &;
    static auto OnSaveCallback() -> OnSaveType &;
};

#endif
