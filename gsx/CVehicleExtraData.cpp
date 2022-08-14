#include "CVehicleExtraData.h"
#include "CExtraSaveData.h"
#include "crc32.h"
#include <cassert>
#include <chrono>
#include <ctime>
#include <plugin_sa/game_sa/CVehicle.h>

static bool floatTest(float f1, float f2) {
    float r = f1 - f2;

    if (r < 0.0f) {
        r *= -1.0f;
    }

    return r < 0.01f;
}

std::string GetDateAndTime() {
    std::time_t tt =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string str = std::string(ctime(&tt));
    if (str[str.size() - 1] == '\n') {
        str.resize(str.size() - 1);
    }
    return str;
}

uint64_t CVehicleExtraData::genHash(int model, int handle, const CVector &pos) {
    float px = pos.x + pos.y + pos.z;

    uint32_t crc[2];

    crc[0] = crc32FromStdString(std::to_string(px) + std::to_string(handle) +
                                std::to_string(model));
    crc[1] = crc32FromStdString(std::to_string(handle) + std::to_string(model) +
                                GetDateAndTime());

    return *reinterpret_cast<uint64_t *>(crc);
}

uint64_t CVehicleExtraData::genHashFromVeh(CVehicle *ptr) {
    return genHash(ptr->m_nModelIndex, CPool_CVehicle__handleOf(getPool(), ptr),
                   ptr->m_matrix->at);
}

CVehicleExtraData::cardata *
CVehicleExtraData::vehicleDataRestore(CVehicle *veh, CStoredCar *storedData) {
    auto &data = getDataByVehPtr(veh);

    auto &allCars = CExtraSaveData::inst().data;

    for (auto it = allCars.begin(); it != allCars.end(); /******/) {
        auto &car = *it;

        /*
        Remove model check to be compatible with autoid
        */
        if (/*car.model == storedData->model &&*/ floatTest(
                car.pos.x, storedData->pos.x) &&
            floatTest(car.pos.y, storedData->pos.y) &&
            floatTest(car.pos.z, storedData->pos.z)) {
            data.destroy();
            data.toLoad = std::move(car.data);

            it = allCars.erase(it);
            return &data;
            break;
        }
        ++it;
    }

    return &data;
}

CVehicleExtraData &CVehicleExtraData::inst() {
    static CVehicleExtraData result;
    return result;
}

void CVehicleExtraData::clearCarsPool() {
    initCarsPool();

    for (int i = 0, size = carsPool.m_nSize; i < size; i++) {
        carsPool.m_pObjects[i].construct();
    }
}

void CVehicleExtraData::refreshCarsPool() {}

void CVehicleExtraData::initCarsPool() {
    if (carsPoolInited) {
        return;
    }

    if (pool == nullptr) {
        pool = *injector::ReadMemory<CPool<CVehicle> **>(0x0055103C + 1);
    }

    carsPool.m_pObjects = new cardata[pool->m_nSize];

    carsPool.m_nSize = pool->m_nSize;
    carsPool.m_nFirstFree = pool->m_nFirstFree;

    carsPool.m_bOwnsAllocations = false;

    carsPool.m_byteMap = pool->m_byteMap;
    carsPoolInited = true;

    for (int i = 0, size = carsPool.m_nSize; i < size; i++) {
        carsPool.m_pObjects[i].construct();
    }
}

void CVehicleExtraData::onloadGame(int id) {
    initCarsPool();
    clearCarsPool();
}

void CVehicleExtraData::onsaveGame(int id) {}

CVehicleExtraData::cardata &CVehicleExtraData::getDataByVehPtr(CVehicle *ptr) {
    int handle = CPool_CVehicle__handleOf(pool, ptr);
    int phandle = handle >> 8;

    auto &obj = carsPool.m_pObjects[phandle];
    return obj;
}

bool CVehicleExtraData::testGSXReserverdNames(CVehicle *veh, const char *name,
                                              cardata &data) {
    if (name[0] == '_') {
        std::string nm = name;

        if (nm == "_hash") {
            if (data.toLoad["_hash"].bytes.empty()) {
                uint64_t chash = CVehicleExtraData::inst().genHashFromVeh(veh);
                data.toLoad["_hash"].bytes.reserve(sizeof(chash));

                uint8_t *phash = reinterpret_cast<uint8_t *>(&chash);

                for (int i = 0; i < sizeof(chash); i++) {
                    data.toLoad["_hash"].bytes.push_back(phash[i]);
                }
            }
            return true;
        }
    }

    return false;
}

auto CVehicleExtraData::getVehicleModel(CVehicle *veh) -> short {
    return veh->m_nModelIndex;
}

extern "C" void __declspec(dllexport) __cdecl setDataToSaveLaterVehPtr(
    CVehicle *veh, const char *name, int size, const void *ptr,
    bool forceCopyNow) {
    if (veh == nullptr) {
        return;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    if (CVehicleExtraData::testGSXReserverdNames(veh, name, data)) {
        return;
    }

    auto &toSave = data.toSave[name];

    toSave.size = size;
    toSave.ptr = const_cast<void *>(ptr);

    if (forceCopyNow) {
        auto &toLoad = data.toLoad[name];
        toLoad.savedRecently = true;

        auto &b = toLoad.bytes;
        b.clear();

        const uint8_t *arr = (const uint8_t *)toSave.ptr;

        b.reserve(toSave.size);

        for (int i = 0; i < toSave.size; i++) {
            b.push_back(arr[i]);
        }
    }
}

extern "C" void __declspec(dllexport) __cdecl pushDirectlyToSavedData(
    CVehicle *veh, const char *name, int size, const void *ptr) {
    if (veh == nullptr) {
        return;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    if (CVehicleExtraData::testGSXReserverdNames(veh, name, data)) {
        return;
    }

    {
        auto &toLoad = data.toLoad[name];
        toLoad.savedRecently = true;

        auto &b = toLoad.bytes;
        b.clear();

        uint8_t *arr = (uint8_t *)ptr;

        b.reserve(size);

        for (int i = 0; i < size; i++) {
            b.push_back(arr[i]);
        }
    }
}

extern "C" int __declspec(dllexport) __cdecl dataToSaveLaterExists(
    CVehicle *veh, const char *name) {
    if (veh == nullptr) {
        return false;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    CVehicleExtraData::testGSXReserverdNames(veh, name, data);

    auto it = data.toSave.find(name);

    if (it != data.toSave.end()) {
        return true;
    }

    return false;
}

extern "C" void __declspec(dllexport) __cdecl removeToLoadDataVehPtr(
    CVehicle *veh, const char *name) {
    if (veh == nullptr) {
        return;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    if (CVehicleExtraData::testGSXReserverdNames(veh, name, data)) {
        return;
    }

    auto it = data.toLoad.find(name);

    if (it != data.toLoad.end()) {
        data.toLoad.erase(it);
    }
}

extern "C" void __declspec(dllexport) __cdecl removeToSaveLaterVehPtr(
    CVehicle *veh, const char *name) {
    if (veh == nullptr) {
        return;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    if (CVehicleExtraData::testGSXReserverdNames(veh, name, data)) {
        return;
    }

    auto it = data.toSave.find(name);

    if (it != data.toSave.end()) {
        data.toSave.erase(it);
    }
}

extern "C" int __declspec(dllexport) __cdecl dataToLoadExists(
    CVehicle *veh, const char *name) {
    if (veh == nullptr) {
        std::map<std::string, toLoadData> *toLoad =
            CVehicleExtraData::inst().beforeSpawned.toLoad;
        if (toLoad) {
            auto it = toLoad->find(name);

            if (it != toLoad->end()) {
                return true;
            }
        }

        return false;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    CVehicleExtraData::testGSXReserverdNames(veh, name, data);

    auto it = data.toLoad.find(name);

    if (it != data.toLoad.end()) {
        return true;
    }

    return false;
}

extern "C" __declspec(dllexport) void __cdecl GSX_setScheduledVehicleData(
    CVehicle *veh, const char *name, int size, const void *ptr,
    bool forceCopyNow) {
    setDataToSaveLaterVehPtr(veh, name, size, ptr, forceCopyNow);
}

extern "C" __declspec(dllexport) void __cdecl GSX_deleteScheduledVehicleData(
    CVehicle *veh, const char *name) {
    removeToSaveLaterVehPtr(veh, name);
}

extern "C" __declspec(dllexport) void __cdecl GSX_setVehicleData(
    CVehicle *veh, const char *name, int size, const void *ptr) {
    pushDirectlyToSavedData(veh, name, size, ptr);
}

extern "C" __declspec(dllexport) void __cdecl GSX_deleteVehicleData(
    CVehicle *veh, const char *name) {
    removeToLoadDataVehPtr(veh, name);
}

extern "C" __declspec(dllexport) uint32_t
    __cdecl GSX_getVehicleData(CVehicle *veh, const char *name, uint8_t *ptr,
                               uint32_t maxsize) {
    if (veh == nullptr) {
        std::map<std::string, toLoadData> *toLoad =
            CVehicleExtraData::inst().beforeSpawned.toLoad;
        if (toLoad) {
            auto it = toLoad->find(name);

            if (it != toLoad->end()) {
                auto &obj = *it;

                if (obj.second.bytes.size() == 0)
                    return 0;

                uint32_t copysz = std::min(obj.second.bytes.size(), maxsize);
                std::copy(obj.second.bytes.data(),
                          obj.second.bytes.data() + copysz, ptr);

                return copysz;
            }
        }

        return 0;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    CVehicleExtraData::testGSXReserverdNames(veh, name, data);

    auto it = data.toLoad.find(name);

    if (it != data.toLoad.end()) {
        auto &obj = *it;

        if (obj.second.bytes.size() == 0)
            return 0;

        uint32_t copysz = std::min(obj.second.bytes.size(), maxsize);
        std::copy(obj.second.bytes.data(), obj.second.bytes.data() + copysz,
                  ptr);

        return copysz;
    }

    return 0;
}

extern "C" __declspec(dllexport) void *__cdecl getLoadDataByVehPtr(
    CVehicle *veh, const char *name) {
    if (veh == nullptr) {
        std::map<std::string, toLoadData> *toLoad =
            CVehicleExtraData::inst().beforeSpawned.toLoad;
        if (toLoad) {
            auto it = toLoad->find(name);

            if (it != toLoad->end()) {
                auto &obj = *it;

                if (obj.second.bytes.size() == 0)
                    return nullptr;

                return &(obj.second.bytes[0]);
            }
        }

        return nullptr;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    CVehicleExtraData::testGSXReserverdNames(veh, name, data);

    auto it = data.toLoad.find(name);

    if (it != data.toLoad.end()) {
        auto &obj = *it;

        if (obj.second.bytes.size() == 0)
            return nullptr;

        return &(obj.second.bytes[0]);
    }

    return nullptr;
}

extern "C" __declspec(dllexport) void *__cdecl getLoadDataByVehPtrSz(
    CVehicle *veh, const char *name, int expectingSize) {
    if (veh == nullptr) {
        std::map<std::string, toLoadData> *toLoad =
            CVehicleExtraData::inst().beforeSpawned.toLoad;
        if (toLoad != nullptr) {
            auto it = toLoad->find(name);

            if (it != toLoad->end()) {
                auto &obj = *it;

                if (obj.second.bytes.empty()) {
                    return nullptr;
                }

                if (obj.second.bytes.size() != expectingSize) {
                    return nullptr;
                }

                return &(obj.second.bytes[0]);
            }
        }

        return nullptr;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    CVehicleExtraData::testGSXReserverdNames(veh, name, data);

    auto it = data.toLoad.find(name);

    if (it != data.toLoad.end()) {
        auto &obj = *it;

        if (obj.second.bytes.empty()) {
            return nullptr;
        }

        if (obj.second.bytes.size() != expectingSize) {
            return nullptr;
        }

        return &(obj.second.bytes[0]);
    }

    return nullptr;
}

extern "C" __declspec(dllexport) int __cdecl getDataToLoadSize(
    CVehicle *veh, const char *name) {
    if (veh == nullptr) {
        std::map<std::string, toLoadData> *toLoad =
            CVehicleExtraData::inst().beforeSpawned.toLoad;
        if (toLoad != nullptr) {
            auto it = toLoad->find(name);

            if (it != toLoad->end()) {
                auto &obj = *it;
                return obj.second.bytes.size();
            }
        }

        return -1;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    CVehicleExtraData::testGSXReserverdNames(veh, name, data);

    auto it = data.toLoad.find(name);

    if (it != data.toLoad.end()) {
        auto &obj = *it;
        return obj.second.bytes.size();
    }

    return -1;
}

extern "C" __declspec(dllexport) CStoredCar *getStoredCarBeforeSpawn() {
    return CVehicleExtraData::inst().beforeSpawned.storedCar;
}

extern "C" __declspec(dllexport) int __cdecl getDataToSaveSize(
    CVehicle *veh, const char *name) {
    if (veh == nullptr) {
        return -1;
    }

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    CVehicleExtraData::testGSXReserverdNames(veh, name, data);

    auto it = data.toSave.find(name);

    if (it != data.toSave.end()) {
        auto &obj = *it;
        return obj.second.size;
    }

    return -1;
}

extern "C" __declspec(dllexport) int __cdecl GSX_hasVehicleData(
    CVehicle *veh, const char *name) {
    int result = getDataToLoadSize(veh, name);
    return result == -1 ? 0 : result;
}

extern "C" __declspec(dllexport) int __cdecl GSX_hasScheduledVehicleData(
    CVehicle *veh, const char *name) {
    int result = getDataToSaveSize(veh, name);
    return result == -1 ? 0 : result;
}

CVehicleExtraData::CVehicleExtraData() {
    carsPoolInited = false;
    pool = nullptr;
}
