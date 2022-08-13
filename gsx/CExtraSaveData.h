#pragma once
#ifndef GTASA_EXTRASAVEDATA
#define GTASA_EXTRASAVEDATA
#define SUPPORTED_10US
#include "LoadSaveStructs.h"
#include <cereal/archives/portable_binary.hpp>
#include <deque>
#include <functional>
#include <map>
#include <plugin_sa/game_sa/CVector.h>
#include <string>
#include <vector>

class CVehicle;

struct car {
    int model;
    CVector pos;
    std::map<std::string, toLoadData> data;

    template <class Archive> void serialize(Archive &archive) {
        archive(model, pos, data);
    }
};

class CExtraSaveData {
    typedef std::function<void(int)> OnLoadGameType;
    typedef std::function<void(int)> OnSaveGameType;

    static OnLoadGameType &OnRestoreCallback();
    static OnSaveGameType &OnSaveCallback();

    static void loads(int id);
    static void saves(int id);

  public:
    std::deque<car> data;

    template <class Archive> void serialize(Archive &archive) { archive(data); }

    static void on_load(const OnLoadGameType &fn) { OnRestoreCallback() = fn; }

    static void on_save(const OnSaveGameType &fn) { OnSaveCallback() = fn; }

    static CExtraSaveData &inst();

  private:
    CExtraSaveData();
};

#endif
