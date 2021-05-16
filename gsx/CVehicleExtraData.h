#pragma once
#ifndef GTASA_EXTRAVEHICLEDATA
#define GTASA_EXTRAVEHICLEDATA
#define GTASA
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <injector/saving.hpp>
#include <injector/injector.hpp>
#include <injector/assembly.hpp>
#include <injector/calling.hpp>
//#include <plugin\plugin.h>
#include <plugin_sa\game_sa\RenderWare.h>
#include <plugin_sa\game_sa\CPool.h>
#include <plugin_sa\game_sa\CVehicle.h>
#include <plugin_sa\game_sa\CAutomobile.h>
#include <plugin_sa\game_sa\CModelInfo.h>
#include <unordered_map>
#include "LoadSaveStructs.h"
#include "CStoredCar.h"

static auto RwMalloc = injector::cstd<void*(size_t)>::call<0x00824257>;
static auto RwFree = injector::cstd<void(void*)>::call<0x0082413F>;
static auto CPool_CVehicle___atHandle = injector::thiscall<CVehicle *(void *, DWORD handle)>::call<0x004048E0>;
static auto CPool_CVehicle__handleOf = injector::thiscall<int(void *, CVehicle *)>::call<0x00424160>;
static auto CModelInfo__GetModelInfo = injector::cstd<void*(char *modelName, int *pIndex)>::call<0x004C5940>;
static auto getVehicleModelInfoByID = injector::cstd<CVehicleModelInfo*(int id)>::call<0x00403DA0>;
static auto CEntity__getColModel = injector::thiscall<CColModel*(CEntity *)>::call<0x00535300>;

class CVehicleExtraData
{
	bool carsPoolInited;
	CPool<CVehicle> *pool;

public:
	struct cardata
	{
		std::map<std::string, toSaveData> toSave;
		std::map<std::string, toLoadData> toLoad;

		void construct()
		{
			toSave.clear();
			toLoad.clear();
		}

		void destroy()
		{
			toSave.clear();
			toLoad.clear();
		}

		cardata()
		{
			construct();
		}

		~cardata()
		{
			destroy();
		}
	};

	struct beforeSpawnData
	{
		std::map<std::string, toLoadData> *toLoad;
		CStoredCar *storedCar;

		beforeSpawnData() : toLoad(nullptr), storedCar(nullptr)
		{

		}
	} beforeSpawned;

	CPool<cardata> carsPool;

	static CVehicleExtraData &inst();

	void onloadGame(int id);
	void onsaveGame(int id);
	cardata &getDataByVehPtr(CVehicle *ptr);
	CPool<CVehicle> *getPool() { return pool; }
	static uint64_t genHash(int model, int handle, const CVector &pos);
	uint64_t genHashFromVeh(CVehicle *ptr);
	cardata *vehicleDataRestore(CVehicle *veh, CStoredCar *storedData);
	static bool testGSXReserverdNames(CVehicle *veh, const char *name, cardata &data);

private:
	void clearCarsPool();

	void refreshCarsPool();
	void initCarsPool();

	CVehicleExtraData();
};

#endif
