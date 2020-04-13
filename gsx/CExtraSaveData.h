#pragma once
#ifndef GTASA_EXTRASAVEDATA
#define GTASA_EXTRASAVEDATA
#include <map>
#include <string>
#include <vector>
#include <deque>
#include <plugin_sa\game_sa\CVector.h>
#include "LoadSaveStructs.h"
#include <cereal\cereal.hpp>
#include <cereal\types\map.hpp>
#include <cereal\types\deque.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\portable_binary.hpp>

struct car
{
	int model;
	CVector pos;
	std::map<std::string, toLoadData> data;

	template<class Archive>
	void load(Archive &archive)
	{
		archive(model, pos, data);
	}

	template<class Archive>
	void save(Archive &archive) const
	{
		archive(model, pos, data);
	}
};

class CExtraSaveData
{
	typedef std::function<void(int)> OnLoadGameType;
	typedef std::function<void(int)> OnSaveGameType;

	static OnLoadGameType& OnRestoreCallback();
	static OnSaveGameType& OnSaveCallback();

	static void loads(int id);
	static void saves(int id);

public:
	std::deque<car> data;

	template<class Archive>
	void load(Archive &archive)
	{
		archive(data);
	}

	template<class Archive>
	void save(Archive &archive) const
	{
		archive(data);
	}
	
	static void on_load(const OnLoadGameType& fn)
	{
		OnRestoreCallback() = fn;
	}

	static void on_save(const OnSaveGameType& fn)
	{
		OnSaveCallback() = fn;
	}

	static CExtraSaveData &inst();

private:
	CExtraSaveData();
};

#endif