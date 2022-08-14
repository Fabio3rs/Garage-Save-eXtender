#pragma once

#include "../injector/saving.hpp"
#include <functional>

class CVehicle;
class CStoredCar;

namespace GSX {
using scopedUserdirFactory =
    std::function<injector::save_manager::scoped_userdir()>;

struct InternalCallbacks {
    static void staticHookClearData();
    static void restore(CVehicle *veh, CStoredCar *storedData);
    static void restoreDataBeforeSpawn(CVehicle *veh, CStoredCar *storedData);
    static void save(CVehicle *veh, CStoredCar *storedData);
    static void onLoadGame(int id);
    static void onSaveGame(int id);

    static scopedUserdirFactory userDirFactory;
};
} // namespace GSX
