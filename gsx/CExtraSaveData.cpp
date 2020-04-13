#include "CExtraSaveData.h"
#include <injector/saving.hpp>
#include <plugin_sa\game_sa\CColData.h>
#include <injector/injector.hpp>
#include <injector/assembly.hpp>
#include <injector/calling.hpp>
//#include <plugin\plugin.h>
#include <plugin_sa\game_sa\RenderWare.h>
#include <map>
#include <plugin_sa\game_sa\CPool.h>
#include <plugin_sa\game_sa\CDummy.h>
#include <plugin_sa\game_sa\CObject.h>
#include <plugin_sa\game_sa\CVehicle.h>
#include <plugin_sa\game_sa\CPed.h>
#include <plugin_sa\game_sa\CAutomobile.h>
#include <vector>
#include <plugin_sa\game_sa\CModelInfo.h>
#include <unordered_map>
#include <fstream>
#include "CVehicleExtraData.h"

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

	std::string name = "gsx/carsExtraData" + std::to_string(id);

	injector::save_manager::scoped_userdir dir;

	std::fstream file(name, std::ios::in | std::ios::binary);

	if (file.is_open())
	{
		try {
			cereal::PortableBinaryInputArchive iarchive(file);

			iarchive(inst());
		}
		catch (...)
		{
			inst().data.clear();
		}
	}
}

void CExtraSaveData::saves(int id)
{
	CVehicleExtraData::inst().onsaveGame(id);

	std::string name = "gsx/carsExtraData" + std::to_string(id);

	injector::save_manager::scoped_userdir dir;

	CreateDirectoryA("gsx", 0);
	std::fstream file(name, std::ios::out | std::ios::trunc | std::ios::binary);


	if (file.is_open())
	{
		try {
			cereal::PortableBinaryOutputArchive iarchive(file);

			iarchive(inst());
		}
		catch (...)
		{
		}
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
