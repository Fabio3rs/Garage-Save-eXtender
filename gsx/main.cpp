#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "grgExtraManager.h"
#include "CVehicleExtraData.h"
#include "CExtraSaveData.h"
#include "CStoredCar.h"
#include <string>
#include <array>
#include <ctime>
#include <chrono>
#include "crc32.h"
#include <plugin_sa/game_sa/CVehicle.h>
#include "CLog.h"

extern "C" __declspec(dllexport) const char * __cdecl GSX_getCompileTime()
{
	return __DATE__ " " __TIME__;
}

extern "C" __declspec(dllexport) const char * __cdecl GSX_getVersionString()
{
	return "0.3.7a";
}

extern "C" __declspec(dllexport) float __cdecl GSX_getVersionNum()
{
	return 0.3705f;
}

extern "C" __declspec(dllexport) int32_t __cdecl GSX_getVersionNumInt32()
{
	return 0x00030705;
}

struct journalNews
{
	CVehicle *veh;
	int32_t status;
	size_t when;

	journalNews()
	{
		veh = nullptr;
		status = 0;
		when = 0;
	}
};

struct apiCarNotify
{
	CVehicle *veh;
	int32_t status;
};

struct externalCallbackStructure
{
	CVehicle *veh;
	int32_t status;
	CStoredCar *gameStoredData;
};

typedef void(__cdecl externalCbFun_t)(const externalCallbackStructure*);
typedef std::vector<std::function<externalCbFun_t>> externalCallbacks_t;

static externalCallbacks_t &getCallbacks()
{
	static externalCallbacks_t externalCb;
	return externalCb;
}

static externalCallbacks_t &getTempCallbacks()
{
	static externalCallbacks_t externalCb;
	return externalCb;
}

static size_t lastCar = 0;

static void onSaveGame(int id)
{

}

void callexternalCallbacks(externalCallbackStructure &e)
{
	for (auto &cb : getCallbacks())
	{
		if (cb) cb(&e);
	}

	for (auto &cb : getTempCallbacks())
	{
		if (cb) cb(&e);
	}
}

extern "C" __declspec(dllexport) int addNotifyCallback(externalCbFun_t fun)
{
	{
		auto &cbs = getCallbacks();

		for (int i = 0, size = cbs.size(); i < size; i++)
		{
			auto &cbRef = cbs[i];
			if (!cbRef)
			{
				cbRef = fun;
				return i;
			}
		}
	}

	getCallbacks().push_back(fun);

	return getCallbacks().size() - 1;
}

extern "C" __declspec(dllexport) int addNotifyTempCallback(externalCbFun_t fun)
{
	{
		auto &cbs = getTempCallbacks();

		for (int i = 0, size = cbs.size(); i < size; i++)
		{
			auto &cbRef = cbs[i];
			if (!cbRef)
			{
				cbRef = fun;
				return i;
			}
		}
	}
	
	getTempCallbacks().push_back(fun);

	return getTempCallbacks().size() - 1;
}

extern "C" __declspec(dllexport) void removeNotifyCallback(int cbRef)
{
	auto &cbs = getCallbacks();

	if (cbs.size() > cbRef && cbRef >= 0)
	{
		cbs[cbRef] = nullptr;
	}
}

extern "C" __declspec(dllexport) void removeNotifyTempCallback(int cbRef)
{
	auto &cbs = getTempCallbacks();

	if (cbs.size() > cbRef && cbRef >= 0)
	{
		cbs[cbRef] = nullptr;
	}
}


typedef std::array<journalNews, 48> journalArray_t;

static journalArray_t &getJournal()
{
	static journalArray_t journalArray;
	return journalArray;
}

static void onLoadGame(int id)
{
	///getTempCallbacks().clear();
	lastCar = 0;

	auto &journal = getJournal();

	journal[0].when = 0;
}

static void addCarToJournalList(journalNews j)
{
	j.when = ++lastCar;

	auto &journal = getJournal();
	std::copy_backward(journal.begin(), std::prev(journal.end()), journal.end());

	journal[0] = j;
}

extern "C" __declspec(dllexport) int getNewCarGrgForeach(size_t *i, apiCarNotify *out)
{
	if (i == nullptr || out == nullptr)
		return false;

	const auto &journal = getJournal();

	if ((*i) == journal[0].when)
	{
		return false;
	}

	++(*i);

	int l = 0, j = 0;

	for (j = 0; j < journal.size(); j++)
	{
		if ((*i) == journal[j].when)
		{
			l = j;
			break;
		}
	}

	if (j >= journal.size())
		return false;

	*out = apiCarNotify();

	out->veh = journal[l].veh;
	out->status = journal[l].status;

	return true;
}

static bool floatTest(float f1, float f2)
{
	float r = f1 - f2;

	if (r < 0.0f)
	{
		r *= -1.0f;
	}

	return r < 0.01f;
}

void hashSet(CVehicleExtraData::cardata &data, CVehicle *veh, CStoredCar *storedData)
{
	{
		auto &car = data.toLoad;

		if (data.toLoad["_hash"].bytes.size() == 0)
		{
			uint64_t chash = CVehicleExtraData::inst().genHash(storedData->model, CPool_CVehicle__handleOf(CVehicleExtraData::inst().getPool(), veh), storedData->pos);

			data.toLoad["_hash"].bytes.reserve(sizeof(chash));

			uint8_t *phash = reinterpret_cast<uint8_t*>(&chash);

			for (int i = 0; i < sizeof(chash); i++)
			{
				data.toLoad["_hash"].bytes.push_back(phash[i]);
			}
		}
	}
}

void restore(CVehicle *veh, CStoredCar *storedData)
{
	{
		CVehicleExtraData::beforeSpawnData &beforeSpawned = CVehicleExtraData::inst().beforeSpawned;
		beforeSpawned.toLoad = nullptr;
		beforeSpawned.storedCar = nullptr;
	}

	{
		CVehicleExtraData::cardata *pdata = CVehicleExtraData::inst().vehicleDataRestore(veh, storedData);

		if (pdata)
			hashSet(*pdata, veh, storedData);
		else
		{

		}

		journalNews j;
		j.veh = veh;
		j.status = 0;
		addCarToJournalList(j);

		{
			externalCallbackStructure strct;
			strct.veh = veh;
			strct.status = 0;
			strct.gameStoredData = storedData;

			callexternalCallbacks(strct);
		}
	}
}

void restoreDataBeforeSpawn(CVehicle *veh, CStoredCar *storedData)
{
	CVehicleExtraData::beforeSpawnData &beforeSpawned = CVehicleExtraData::inst().beforeSpawned;
	beforeSpawned.toLoad = nullptr;
	beforeSpawned.storedCar = nullptr;

	{
		auto &allCars = CExtraSaveData::inst().data;

		CVehicleExtraData::inst().vehicleDataRestore(veh, storedData);

		//if (beforeSpawned.toLoad)
		{
			externalCallbackStructure strct;
			strct.veh = veh;
			strct.status = 2;
			strct.gameStoredData = storedData;

			callexternalCallbacks(strct);
		}
	}
}

void save(CVehicle *veh, CStoredCar *storedData)
{
	{
		externalCallbackStructure strct;
		strct.veh = veh;
		strct.status = 1;
		strct.gameStoredData = storedData;

		callexternalCallbacks(strct);
	}

	auto &data = CVehicleExtraData::inst().getDataByVehPtr(veh);

	{
		for (auto &tsave : data.toSave)
		{
			auto &b = data.toLoad[tsave.first].bytes;

			const uint8_t *arr = (const uint8_t*)tsave.second.ptr;

			b.clear();
			b.reserve(tsave.second.size);

			for (int i = 0; i < tsave.second.size; i++)
			{
				b.push_back(arr[i]);
			}
		}

		if (data.toLoad.size() > 0)
		{
			car c;
			c.model = veh->m_nModelIndex;
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

typedef injector::function_hooker<injector::scoped_call, 0x0053BCC9, void(void)> ngamectscptp_hook;
typedef injector::function_hooker<injector::scoped_call, 0x00618F05, void(void)> ngamectscptp_hook2;
typedef injector::function_hooker<injector::scoped_call, 0x0053BE8D, void(void)> ngamectscptp_hook3;

void staticHookClearData()
{
	getTempCallbacks().clear();
}

void hook()
{
	CLog::log() << "void hook() function";
	CExtraSaveData::inst();
	grgExtraManager::grg();
	grgExtraManager::on_restore(restore);
	grgExtraManager::on_restore_bef(restoreDataBeforeSpawn);
	grgExtraManager::on_save(save);

	CVehicleExtraData::inst();


	CExtraSaveData::inst().on_load(onLoadGame);
	CExtraSaveData::inst().on_save(onSaveGame);

	{
		injector::make_static_hook<ngamectscptp_hook>([](ngamectscptp_hook::func_type func)
		{
			staticHookClearData();
			return func();
		});

		injector::make_static_hook<ngamectscptp_hook2>([](ngamectscptp_hook2::func_type func)
		{
			staticHookClearData();
			return func();
		});

		injector::make_static_hook<ngamectscptp_hook3>([](ngamectscptp_hook3::func_type func)
		{
			staticHookClearData();
			return func();
		});
	}
}

BOOL WINAPI DllMain(
	_In_  HINSTANCE hinstDLL,
	_In_  DWORD fdwReason,
	_In_  LPVOID lpvReserved
	){
	if (fdwReason == DLL_PROCESS_ATTACH) hook();


	return true;
}