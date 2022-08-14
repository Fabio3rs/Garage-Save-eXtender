#pragma once
#ifndef GARAGESAVEEXTENDERAPI_H
#define GARAGESAVEEXTENDERAPI_H
#include <cstdint>
#include <windows.h>

class CVehicle;
class CStoredCar;

// Plugin-SDK dependent headers
#include "CStoredCar.h"

namespace GSX {
auto setASIPath(const wchar_t *path)
    -> const wchar_t *; // Don't use temporary pointer

const static uint32_t LOAD_CAR = 0, SAVE_CAR = 1, LOAD_CAR_BEFORE_SPAWN = 2;

struct journalNews {
    CVehicle *veh;
    int32_t status;
    size_t when;

    journalNews() {
        veh = nullptr;
        status = 0;
        when = 0;
    }
};

struct apiCarNotify {
    CVehicle *veh;
    int32_t status;
};

struct externalCallbackStructure {
    CVehicle *veh;
    int32_t status;
    CStoredCar *gameStoredData;
};

typedef void(__cdecl externalCbFun_t)(const externalCallbackStructure *);

// Callbacks
void addNotifyCallback(externalCbFun_t fun);
void addNotifyTempCallback(
    externalCbFun_t fun); // On game reload, temp callbacks will be erased

// Manual loop
bool getNewCarForeach(size_t &i, apiCarNotify &out);

// Save data
void setDataToSaveLater(CVehicle *veh, const char *name, int size, void *ptr,
                        bool forceCopyNow);

template <class T>
void setDataToSaveLater(CVehicle *veh, const char *name, const T &var,
                        bool forceCopyNow) {
    setDataToSaveLater(veh, name, sizeof(var), std::addressof(var),
                       forceCopyNow);
}

void pushDirectlyToSavedData(CVehicle *veh, const char *name, int size,
                             void *ptr);

template <class T>
void pushDirectlyToSavedData(CVehicle *veh, const char *name, const T &var) {
    pushDirectlyToSavedData(veh, name, sizeof(var), std::addressof(var));
}

bool dataToSaveLaterExists(CVehicle *veh, const char *name);
bool dataToLoadExists(CVehicle *veh, const char *name);

// Remove data
void removeToSaveLaterData(CVehicle *veh, const char *name);
void removeSavedData(CVehicle *veh, const char *name);

// Get data info
void *getSavedData(CVehicle *veh, const char *name);
void *getSavedDataSz(CVehicle *veh, const char *name, int expectingSize);

template <class T> T *getSavedDataT(CVehicle *veh, const char *name) {
    return reinterpret_cast<T *>(getSavedDataSz(veh, name, sizeof(T)));
}

int getDataToLoadSize(CVehicle *veh, const char *name);
int getDataToSaveSize(CVehicle *veh, const char *name);

// CStoredCar *getStoredCarBeforeSpawn();

// ASI version
const char *getCompileTime();
const char *getVersionString();
float getVersionNum();

// API version
const char *getAPIVersionString();

/// New API Functions
void setScheduledVehicleData(CVehicle *veh, const char *name, int size,
                             const void *ptr, bool forceCopyNow);
void deleteScheduledVehicleData(CVehicle *veh, const char *name);
void setVehicleData(CVehicle *veh, const char *name, int size, const void *ptr);
void deleteVehicleData(CVehicle *veh, const char *name);
uint32_t getVehicleData(CVehicle *veh, const char *name, uint8_t *ptr,
                        uint32_t maxsize);
int hasVehicleData(CVehicle *veh, const char *name);
int hasScheduledVehicleData(CVehicle *veh, const char *name);
} // namespace GSX

#endif
