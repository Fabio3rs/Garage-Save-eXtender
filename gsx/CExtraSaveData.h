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
    using OnLoadGameType = std::function<void(int)>;
    using OnSaveGameType = std::function<void(int)>;

    static auto OnRestoreCallback() -> OnLoadGameType &;
    static auto OnSaveCallback() -> OnSaveGameType &;

  public:
    void loads(int id, std::istream &in);
    void saves(int id, std::ostream &out);

    std::deque<car> data;

    template <class Archive> void serialize(Archive &archive) { archive(data); }

    static void on_load(const OnLoadGameType &fn) { OnRestoreCallback() = fn; }

    static void on_save(const OnSaveGameType &fn) { OnSaveCallback() = fn; }

    static auto inst() -> CExtraSaveData &;

  private:
    CExtraSaveData();
};

#endif
