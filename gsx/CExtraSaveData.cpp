#include "CExtraSaveData.h"
#include "../injector/saving.hpp"
#include "CLog.h"
#include <cereal/cereal.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <map>
#include <unordered_map>
#include <vector>

namespace cereal {
template <class Archive>
inline void CEREAL_SAVE_FUNCTION_NAME(Archive &ar, const CVector &vector) {
    ar(vector.x, vector.y, vector.z);
}

template <class Archive>
inline void CEREAL_LOAD_FUNCTION_NAME(Archive &ar, CVector &vector) {
    ar(vector.x, vector.y, vector.z);
}
} // namespace cereal

auto CExtraSaveData::OnRestoreCallback() -> CExtraSaveData::OnLoadGameType & {
    static OnLoadGameType cb;
    return cb;
}

auto CExtraSaveData::OnSaveCallback() -> CExtraSaveData::OnSaveGameType & {
    static OnSaveGameType cb;
    return cb;
}

void CExtraSaveData::loads(int id, std::istream &in) {
    data.clear();

    if (auto &fncb = OnRestoreCallback()) {
        fncb(id);
    }

    if (id == -1) {
        return;
    }

    try {
        cereal::PortableBinaryInputArchive iarchive(in);

        iarchive(*this);
    } catch (...) {
        CLog::log().multiRegister("Error file %0 line 1", __FILE__, __LINE__);
        data.clear();
    }
}

void CExtraSaveData::saves(int id, std::ostream &out) {
    try {
        cereal::PortableBinaryOutputArchive iarchive(out);

        iarchive(*this);
    } catch (const std::exception &e) {
        CLog::log().multiRegister("Error %0 %1 line %2", e.what(), __FILE__,
                                  __LINE__);
    } catch (...) {
        CLog::log().multiRegister("Error Unknown error %0 line %1", __FILE__,
                                  __LINE__);
    }

    if (auto &fncb = OnSaveCallback()) {
        fncb(id);
    }
}

auto CExtraSaveData::inst() -> CExtraSaveData & {
    static CExtraSaveData instance;
    return instance;
}

CExtraSaveData::CExtraSaveData() = default;
