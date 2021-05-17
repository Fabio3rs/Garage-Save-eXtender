#include "CExtraSaveData.h"
#include <injector/saving.hpp>
#include <injector/injector.hpp>
#include <injector/assembly.hpp>
#include <injector/calling.hpp>
//#include <plugin\plugin.h>
#include <plugin_sa\game_sa\RenderWare.h>
#include <map>
#include <plugin_sa\game_sa\CPool.h>
#include <plugin_sa\game_sa\CVehicle.h>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "CVehicleExtraData.h"
#include "CLog.h"

CPool<CVehicle> *pool;

//static auto chdirUserDir = injector::cstd<int()>::call<0x00538860>;
//static auto CFileMgr__SetDir = injector::cstd<int(const char *relPath)>::call<0x005387D0>;

CExtraSaveData::OnLoadGameType &CExtraSaveData::OnRestoreCallback()
{
	static OnLoadGameType cb;
	return cb;
}

CExtraSaveData::OnSaveGameType &CExtraSaveData::OnSaveCallback()
{
	static OnSaveGameType cb;
	return cb;
}

void CExtraSaveData::loads(int id)
{
	CVehicleExtraData::inst().onloadGame(id);
	inst().data.clear();

	{
		auto &fncb = OnRestoreCallback();
		if (fncb)
		{
			fncb(id);
		}
	}

	if (id == -1)
		return;

	injector::save_manager::scoped_userdir dir;

	std::string name;
	name.reserve(4096);
	{
		char buffer[4096] = { 0 };
		GetCurrentDirectoryA(sizeof(buffer), buffer);
		name += buffer;
	}
	name += "/gsx/carsExtraData" + std::to_string(id);

	std::fstream file(name, std::ios::in | std::ios::binary);

	if (file.is_open())
	{
		try {
			cereal::PortableBinaryInputArchive iarchive(file);

			iarchive(inst());
		}
		catch (...)
		{
			CLog::log().multiRegister("Error file %0 line 1", __FILE__, __LINE__);
			inst().data.clear();
		}
	}
	else
	{
		CLog::log().multiRegister("Can't open the file %0 line %1 filename: %2", __FILE__, __LINE__, name);
	}
}

void CExtraSaveData::saves(int id)
{
	CVehicleExtraData::inst().onsaveGame(id);

	injector::save_manager::scoped_userdir dir;

	std::string name;
	name.reserve(4096);
	{
		char buffer[4096] = { 0 };
		GetCurrentDirectoryA(sizeof(buffer), buffer);
		name += buffer;
	}
	name += "/gsx/carsExtraData" + std::to_string(id);

	BOOL gsxdir = CreateDirectoryA("gsx", 0);
	std::fstream file(name, std::ios::out | std::ios::trunc | std::ios::binary);


	if (file.is_open())
	{
		try {
			cereal::PortableBinaryOutputArchive iarchive(file);

			iarchive(inst());
		}
		catch (const std::exception &e)
		{
			CLog::log().multiRegister("Error %0 %1 line %2", e.what(), __FILE__, __LINE__);
		}
		catch (...)
		{
			CLog::log().multiRegister("Error Unknown error %0 line %1", __FILE__, __LINE__);
		}
	}
	else
	{
		CLog::log().multiRegister("Can't open the file %0 line %1 filename: %2 CreateDirectoryA: %3", __FILE__, __LINE__, name, (bool)gsxdir);
	}

	{
		auto &fncb = OnSaveCallback();
		if (fncb)
		{
			fncb(id);
		}
	}
}

CExtraSaveData &CExtraSaveData::inst()
{
	static CExtraSaveData instance;
	return instance;
}

CExtraSaveData::CExtraSaveData()
{
	injector::save_manager::on_load(loads);
	injector::save_manager::on_save(saves);

}
