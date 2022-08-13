#pragma once
#ifndef GTASA_EXTRAVEHICLEDATA
#define GTASA_EXTRAVEHICLEDATA
#include "CStoredCar.h"
#include "LoadSaveStructs.h"
#include <cstdint>
#include <injector/calling.hpp>
//#include <injector/saving.hpp>
#include <map>
#include <plugin_sa/game_sa/CPool.h>
#include <string>
#include <unordered_map>
#include <vector>

class CVehicle;
class CVehicleModelInfo;
class CColModel;
class CEntity;

namespace {
/*auto RwMalloc = injector::cstd<void *(size_t)>::call<0x00824257>;
auto RwFree = injector::cstd<void(void *)>::call<0x0082413F>;
auto CPool_CVehicle___atHandle =
    injector::thiscall<CVehicle *(void *, DWORD handle)>::call<0x004048E0>;*/
auto CPool_CVehicle__handleOf =
    injector::thiscall<int(void *, CVehicle *)>::call<0x00424160>;
/*auto CModelInfo__GetModelInfo =
    injector::cstd<void *(char *modelName, int *pIndex)>::call<0x004C5940>;
auto getVehicleModelInfoByID =
    injector::cstd<CVehicleModelInfo *(int id)>::call<0x00403DA0>;
auto CEntity__getColModel =
    injector::thiscall<CColModel *(CEntity *)>::call<0x00535300>;*/
} // namespace

class CVehicleExtraData {
    bool carsPoolInited{false};
    CPool<CVehicle> *pool{nullptr};

  public:
    struct cardata {
        std::map<std::string, toSaveData> toSave;
        std::map<std::string, toLoadData> toLoad;

        void construct() {
            toSave.clear();
            toLoad.clear();
        }

        void destroy() {
            toSave.clear();
            toLoad.clear();
        }
    };

    struct beforeSpawnData {
        std::map<std::string, toLoadData> *toLoad{};
        CStoredCar *storedCar{};
    } beforeSpawned;

    CPool<cardata> carsPool;

    static auto inst() -> CVehicleExtraData &;

    void onloadGame(int id);
    void onsaveGame(int id);
    auto getDataByVehPtr(CVehicle *ptr) -> cardata &;
    auto getPool() -> CPool<CVehicle> * { return pool; }
    static auto genHash(int model, int handle, const CVector &pos) -> uint64_t;
    auto genHashFromVeh(CVehicle *ptr) -> uint64_t;
    auto vehicleDataRestore(CVehicle *veh, CStoredCar *storedData) -> cardata *;
    static auto testGSXReserverdNames(CVehicle *veh, const char *name,
                                      cardata &data) -> bool;

    static auto getVehicleModel(CVehicle *veh) -> short;

  private:
    void clearCarsPool();

    void refreshCarsPool();
    void initCarsPool();

    CVehicleExtraData();
};

#endif
