#include "GSXAPI.h"

extern "C" {
	static void(__cdecl* addNotifyCallback)(GSX::externalCbFun_t fun) = nullptr;
	static void(__cdecl* addNotifyTempCallback)(GSX::externalCbFun_t fun) = nullptr;
	static int(__cdecl* getNewCarGrgForeach)(size_t* i, GSX::apiCarNotify* out) = nullptr;
	static void(__cdecl* setDataToSaveLaterVehPtr)(CVehicle* veh, const char* name, int size, void* ptr, bool forceCopyNow) = nullptr;
	static void(__cdecl* pushDirectlyToSavedData)(CVehicle* veh, const char* name, int size, void* ptr) = nullptr;
	static int(__cdecl* dataToSaveLaterExists)(CVehicle* veh, const char* name) = nullptr;
	static void(__cdecl* removeToLoadDataVehPtr)(CVehicle* veh, const char* name) = nullptr;
	static void(__cdecl* removeToSaveLaterVehPtr)(CVehicle* veh, const char* name) = nullptr;
	static int(__cdecl* dataToLoadExists)(CVehicle* veh, const char* name) = nullptr;
	static void* (__cdecl* getLoadDataByVehPtr)(CVehicle* veh, const char* name) = nullptr;
	static void* (__cdecl* getLoadDataByVehPtrSz)(CVehicle* veh, const char* name, int expectingSize) = nullptr;
	static const char* (__cdecl* GSX_getCompileTime)() = nullptr;
	static const char* (__cdecl* GSX_getVersionString)() = nullptr;
	static float(__cdecl* GSX_getVersionNum)() = nullptr;
	static int(__cdecl* getDataToLoadSize)(CVehicle* veh, const char* name) = nullptr;
	static int(__cdecl* getDataToSaveSize)(CVehicle* veh, const char* name) = nullptr;

	static void(__cdecl* GSX_setScheduledVehicleData)(CVehicle* veh, const char* name, int size, const void* ptr, bool forceCopyNow) = nullptr;
	static void(__cdecl* GSX_deleteScheduledVehicleData)(CVehicle* veh, const char* name) = nullptr;
	static void(__cdecl* GSX_setVehicleData)(CVehicle* veh, const char* name, int size, const void* ptr) = nullptr;
	static void(__cdecl* GSX_deleteVehicleData)(CVehicle* veh, const char* name) = nullptr;
	static uint32_t(__cdecl* GSX_getVehicleData)(CVehicle* veh, const char* name, uint8_t* ptr, uint32_t maxsize) = nullptr;
	static int(__cdecl* GSX_hasScheduledVehicleData)(CVehicle* veh, const char* name) = nullptr;
	static int(__cdecl* GSX_hasVehicleData)(CVehicle* veh, const char* name) = nullptr;
	//static CStoredCar*(__cdecl *getStoredCarBeforeSpawn)() = nullptr;
}

static HMODULE gsxAsi = nullptr;
static wchar_t* GSXPath = L"gsx.asi";

wchar_t* GSX::setASIPath(wchar_t* path)
{
	if (path == nullptr)
		return GSXPath;

	wchar_t* oldPath = GSXPath;
	GSXPath = path;

	return oldPath;
}

template <class T>
static inline void initFun(T& fun, HMODULE lib, const char* name)
{
	fun = (T)GetProcAddress(lib, name);
}

static void load()
{
	static bool loaded = false;
	if (!loaded)
	{
		gsxAsi = LoadLibraryW(GSXPath);

		if (gsxAsi)
			loaded = true;

		initFun(::addNotifyCallback, gsxAsi, "addNotifyCallback");
		initFun(::addNotifyTempCallback, gsxAsi, "addNotifyTempCallback");
		initFun(::getNewCarGrgForeach, gsxAsi, "getNewCarGrgForeach");
		initFun(::setDataToSaveLaterVehPtr, gsxAsi, "setDataToSaveLaterVehPtr");
		initFun(::pushDirectlyToSavedData, gsxAsi, "pushDirectlyToSavedData");
		initFun(::dataToSaveLaterExists, gsxAsi, "dataToSaveLaterExists");
		initFun(::removeToLoadDataVehPtr, gsxAsi, "removeToLoadDataVehPtr");
		initFun(::removeToSaveLaterVehPtr, gsxAsi, "removeToSaveLaterVehPtr");
		initFun(::dataToLoadExists, gsxAsi, "dataToLoadExists");
		initFun(::getLoadDataByVehPtr, gsxAsi, "getLoadDataByVehPtr");
		initFun(::getLoadDataByVehPtrSz, gsxAsi, "getLoadDataByVehPtrSz");
		initFun(::GSX_getCompileTime, gsxAsi, "GSX_getCompileTime");
		initFun(::GSX_getVersionString, gsxAsi, "GSX_getVersionString");
		initFun(::GSX_getVersionNum, gsxAsi, "GSX_getVersionNum");
		initFun(::getDataToLoadSize, gsxAsi, "getDataToLoadSize");
		initFun(::getDataToSaveSize, gsxAsi, "getDataToSaveSize");
		//initFun(::getStoredCarBeforeSpawn, gsxAsi, "getStoredCarBeforeSpawn");
		initFun(::GSX_setScheduledVehicleData, gsxAsi, "GSX_setScheduledVehicleData");
		initFun(::GSX_deleteScheduledVehicleData, gsxAsi, "GSX_deleteScheduledVehicleData");
		initFun(::GSX_setVehicleData, gsxAsi, "GSX_setVehicleData");
		initFun(::GSX_deleteVehicleData, gsxAsi, "GSX_deleteVehicleData");
		initFun(::GSX_getVehicleData, gsxAsi, "GSX_getVehicleData");
		initFun(::GSX_hasScheduledVehicleData, gsxAsi, "GSX_hasScheduledVehicleData");
		initFun(::GSX_hasVehicleData, gsxAsi, "GSX_hasVehicleData");
	}
}

void GSX::addNotifyCallback(externalCbFun_t fun)
{
	load();
	::addNotifyCallback(fun);
}

void GSX::addNotifyTempCallback(externalCbFun_t fun)
{
	load();
	::addNotifyCallback(fun);
}

bool GSX::getNewCarForeach(size_t& i, apiCarNotify& out)
{
	load();
	return (::getNewCarGrgForeach(&i, &out)) != 0;
}

void GSX::setDataToSaveLater(CVehicle* veh, const char* name, int size, void* ptr, bool forceCopyNow)
{
	load();
	::setDataToSaveLaterVehPtr(veh, name, size, ptr, forceCopyNow);
}

void GSX::pushDirectlyToSavedData(CVehicle* veh, const char* name, int size, void* ptr)
{
	load();
	::pushDirectlyToSavedData(veh, name, size, ptr);
}

bool GSX::dataToSaveLaterExists(CVehicle* veh, const char* name)
{
	load();
	return (::dataToSaveLaterExists(veh, name)) != 0;
}

bool GSX::dataToLoadExists(CVehicle* veh, const char* name)
{
	load();
	return (::dataToLoadExists(veh, name)) != 0;
}

void GSX::removeToSaveLaterData(CVehicle* veh, const char* name)
{
	load();
	::removeToSaveLaterVehPtr(veh, name);
}

void GSX::removeSavedData(CVehicle* veh, const char* name)
{
	load();
	::removeToLoadDataVehPtr(veh, name);
}

void* GSX::getSavedData(CVehicle* veh, const char* name)
{
	load();
	return ::getLoadDataByVehPtr(veh, name);
}

void* GSX::getSavedDataSz(CVehicle* veh, const char* name, int expectingSize)
{
	load();
	return ::getLoadDataByVehPtrSz(veh, name, expectingSize);
}

int GSX::getDataToLoadSize(CVehicle* veh, const char* name)
{
	load();
	return ::getDataToLoadSize(veh, name);
}

int GSX::getDataToSaveSize(CVehicle* veh, const char* name)
{
	load();
	return ::getDataToSaveSize(veh, name);
}

/*CStoredCar *GSX::getStoredCarBeforeSpawn()
{
	return ::getStoredCarBeforeSpawn();
}*/

const char* GSX::getCompileTime()
{
	load();
	return GSX_getCompileTime();
}

const char* GSX::getVersionString()
{
	load();
	return GSX_getVersionString();
}

float GSX::getVersionNum()
{
	load();
	return GSX_getVersionNum();
}

const char* GSX::getAPIVersionString()
{
	return "0.3a";
}

void GSX::setScheduledVehicleData(CVehicle* veh, const char* name, int size, const void* ptr, bool forceCopyNow)
{
	load();
	GSX_setScheduledVehicleData(veh, name, size, ptr, forceCopyNow);
}

void GSX::deleteScheduledVehicleData(CVehicle* veh, const char* name)
{
	load();
	GSX_deleteScheduledVehicleData(veh, name);
}

void GSX::setVehicleData(CVehicle* veh, const char* name, int size, const void* ptr)
{
	load();
	GSX_setVehicleData(veh, name, size, ptr);
}

void GSX::deleteVehicleData(CVehicle* veh, const char* name)
{
	load();
	GSX_deleteVehicleData(veh, name);
}

uint32_t GSX::getVehicleData(CVehicle* veh, const char* name, uint8_t* ptr, uint32_t maxsize)
{
	load();
	return GSX_getVehicleData(veh, name, ptr, maxsize);
}

int GSX::hasVehicleData(CVehicle* veh, const char* name)
{
	load();
	return GSX_hasVehicleData(veh, name);
}

int GSX::hasScheduledVehicleData(CVehicle* veh, const char* name)
{
	load();
	return GSX_hasScheduledVehicleData(veh, name);
}
