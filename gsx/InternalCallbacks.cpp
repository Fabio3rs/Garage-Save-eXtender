#include "InternalCallbacks.hpp"
#include "CExtraSaveData.h"
#include "CStoredCar.h"
#include "CVehicleExtraData.h"
#include "CustomSaveFile.hpp"
#include <cstdint>
#include <functional>
#include <vector>

GSX::scopedUserdirFactory GSX::InternalCallbacks::userDirFactory = []() {
    return injector::save_manager::scoped_userdir();
};

struct journalNews {
    CVehicle *veh{};
    int32_t status{};
    size_t when{};
};

struct apiCarNotify {
    CVehicle *veh{};
    int32_t status{};
};

struct externalCallbackStructure {
    CVehicle *veh{};
    int32_t status{};
    CStoredCar *gameStoredData{};
};

using externalCbFun_t =
    __attribute__((cdecl)) void(const externalCallbackStructure *);
using externalCallbacks_t = std::vector<std::function<externalCbFun_t>>;

static auto getCallbacks() -> externalCallbacks_t & {
    static externalCallbacks_t externalCb;
    return externalCb;
}

static auto getTempCallbacks() -> externalCallbacks_t & {
    static externalCallbacks_t externalCb;
    return externalCb;
}

static size_t lastCar = 0;

void GSX::InternalCallbacks::onSaveGame(int id) {
    auto userdir = scopedUserdirFactory();

    auto saveFile = CustomSaveFile::openGsxSaveFile(
        id, std::ios::out | std::ios::trunc | std::ios::binary);
    CVehicleExtraData::inst().onsaveGame(id);
    CExtraSaveData::inst().saves(id, saveFile);
}

void callexternalCallbacks(externalCallbackStructure &e) {
    for (auto &cb : getCallbacks()) {
        if (cb) {
            cb(&e);
        }
    }

    for (auto &cb : getTempCallbacks()) {
        if (cb) {
            cb(&e);
        }
    }
}

using journalArray_t = std::array<journalNews, 48>;

static auto getJournal() -> journalArray_t & {
    static journalArray_t journalArray;
    return journalArray;
}

void GSX::InternalCallbacks::onLoadGame(int id) {
    /// getTempCallbacks().clear();
    auto userdir = scopedUserdirFactory();

    auto saveFile = CustomSaveFile::openGsxSaveFile(id);
    CVehicleExtraData::inst().onloadGame(id);
    CExtraSaveData::inst().loads(id, saveFile);

    lastCar = 0;

    auto &journal = getJournal();

    journal[0].when = 0;
}

static void addCarToJournalList(journalNews j) {
    j.when = ++lastCar;

    auto &journal = getJournal();
    std::copy_backward(journal.begin(), std::prev(journal.end()),
                       journal.end());

    journal[0] = j;
}

extern "C" __declspec(dllexport) auto getNewCarGrgForeach(size_t *i,
                                                          apiCarNotify *out)
    -> int {
    if (i == nullptr || out == nullptr) {
        return 0;
    }

    const auto &journal = getJournal();

    if ((*i) == journal[0].when) {
        return 0;
    }

    ++(*i);

    int l = 0;
    int j = 0;

    for (j = 0; j < journal.size(); j++) {
        if ((*i) == journal[j].when) {
            l = j;
            break;
        }
    }

    if (j >= journal.size()) {
        return 0;
    }

    *out = apiCarNotify();

    out->veh = journal[l].veh;
    out->status = journal[l].status;

    return 1;
}

static auto floatTest(float f1, float f2) -> bool {
    float r = f1 - f2;

    if (r < 0.0F) {
        r *= -1.0F;
    }

    return r < 0.01F;
}

void hashSet(CVehicleExtraData::cardata &data, CVehicle *veh,
             CStoredCar *storedData) {
    auto &car = data.toLoad;

    if (data.toLoad["_hash"].bytes.empty()) {
        uint64_t chash = CVehicleExtraData::genHash(
            storedData->model,
            CPool_CVehicle__handleOf(CVehicleExtraData::inst().getPool(), veh),
            storedData->pos);

        data.toLoad["_hash"].bytes.reserve(sizeof(chash));

        const auto *phash = reinterpret_cast<uint8_t *>(&chash);

        for (int i = 0; i < sizeof(chash); i++) {
            data.toLoad["_hash"].bytes.push_back(phash[i]);
        }
    }
}

void GSX::InternalCallbacks::restore(CVehicle *veh, CStoredCar *storedData) {
    {
        CVehicleExtraData::beforeSpawnData &beforeSpawned =
            CVehicleExtraData::inst().beforeSpawned;
        beforeSpawned.toLoad = nullptr;
        beforeSpawned.storedCar = nullptr;
    }

    {
        CVehicleExtraData::cardata *pdata =
            CVehicleExtraData::inst().vehicleDataRestore(veh, storedData);

        if (pdata != nullptr) {
            hashSet(*pdata, veh, storedData);
        } else {
        }

        journalNews j;
        j.veh = veh;
        j.status = 0;
        addCarToJournalList(j);

        {
            externalCallbackStructure strct{};
            strct.veh = veh;
            strct.status = 0;
            strct.gameStoredData = storedData;

            callexternalCallbacks(strct);
        }
    }
}

void GSX::InternalCallbacks::restoreDataBeforeSpawn(CVehicle *veh,
                                                    CStoredCar *storedData) {
    CVehicleExtraData::beforeSpawnData &beforeSpawned =
        CVehicleExtraData::inst().beforeSpawned;
    beforeSpawned.toLoad = nullptr;
    beforeSpawned.storedCar = nullptr;

    {
        auto &allCars = CExtraSaveData::inst().data;

        CVehicleExtraData::inst().vehicleDataRestore(veh, storedData);

        // if (beforeSpawned.toLoad)
        {
            externalCallbackStructure strct{};
            strct.veh = veh;
            strct.status = 2;
            strct.gameStoredData = storedData;

            callexternalCallbacks(strct);
        }
    }
}

void doCallbacksForSaving(CVehicle *&veh, CStoredCar *storedData) {
    externalCallbackStructure strct{};
    strct.veh = veh;
    strct.status = 1;
    strct.gameStoredData = storedData;

    callexternalCallbacks(strct);
}

void GSX::InternalCallbacks::save(CVehicle *veh, CStoredCar *storedData) {
    doCallbacksForSaving(veh, storedData);

    auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

    {
        for (auto &tsave : data.toSave) {
            auto &b = data.toLoad[tsave.first].bytes;

            const auto *arr = (const uint8_t *)tsave.second.ptr;

            b.clear();
            b.reserve(tsave.second.size);

            for (int i = 0; i < tsave.second.size; i++) {
                b.push_back(arr[i]);
            }
        }
        if (!data.toLoad.empty()) {
            car c;
            c.model = CVehicleExtraData::getVehicleModel(veh); /*veh->m_nModelIndex;*/
            c.pos = storedData->pos;
            c.data = std::move(data.toLoad);

            CExtraSaveData::inst().data.push_back(c);

            journalNews j;
            j.veh = veh;
            j.status = 1;
            addCarToJournalList(j);
        }

        data.toSave.clear();
    }
}

extern "C" __declspec(dllexport) auto addNotifyCallback(externalCbFun_t fun)
    -> int {
    {
        auto &cbs = getCallbacks();

        for (int i = 0, size = cbs.size(); i < size; i++) {
            auto &cbRef = cbs[i];
            if (!cbRef) {
                cbRef = fun;
                return i;
            }
        }
    }

    getCallbacks().push_back(fun);

    return getCallbacks().size() - 1;
}

extern "C" __declspec(dllexport) auto addNotifyTempCallback(externalCbFun_t fun)
    -> int {
    {
        auto &cbs = getTempCallbacks();

        for (int i = 0, size = cbs.size(); i < size; i++) {
            auto &cbRef = cbs[i];
            if (!cbRef) {
                cbRef = fun;
                return i;
            }
        }
    }

    getTempCallbacks().push_back(fun);

    return getTempCallbacks().size() - 1;
}

extern "C" __declspec(dllexport) void removeNotifyCallback(int cbRef) {
    auto &cbs = getCallbacks();

    if (cbs.size() > cbRef && cbRef >= 0) {
        cbs[cbRef] = nullptr;
    }
}

extern "C" __declspec(dllexport) void removeNotifyTempCallback(int cbRef) {
    auto &cbs = getTempCallbacks();

    if (cbs.size() > cbRef && cbRef >= 0) {
        cbs[cbRef] = nullptr;
    }
}

void GSX::InternalCallbacks::staticHookClearData() {
    getTempCallbacks().clear();
}
