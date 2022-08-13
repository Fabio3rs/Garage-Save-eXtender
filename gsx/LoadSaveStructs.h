#pragma once
#ifndef GTASA_LOADSAVEDATASTRUCTS
#define GTASA_LOADSAVEDATASTRUCTS

#include <cereal/archives/portable_binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cstdint>
#include <map>
#include <vector>

struct toLoadData {
    bool savedRecently;
    std::vector<uint8_t> bytes;

    template <class Archive> void load(Archive &archive) {
        archive(savedRecently, bytes);
    }

    template <class Archive> void save(Archive &archive) const {
        archive(savedRecently, bytes);
    }

    toLoadData() { savedRecently = false; }
};

struct toSaveData {
    int size;
    void *ptr;

    toSaveData() {
        size = 0;
        ptr = nullptr;
    }
};

#endif