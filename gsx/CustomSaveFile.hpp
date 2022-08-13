#pragma once

#include <array>
#include <fstream>
#include <windows.h>

namespace GSX {
struct CustomSaveFile {
    static auto concatName(int id) {
        std::string name;
        name.reserve(4096);
        {
            std::array<char, 4096> buffer = {0};
            GetCurrentDirectoryA(buffer.size(), buffer.data());
            name += buffer.data();
        }
        name += "/gsx/carsExtraData";
        name += std::to_string(id);

        return name;
    }

    static auto
    openGsxSaveFile(int id, std::ios_base::openmode flags = std::ios::in |
                                                            std::ios::binary) {
        return std::fstream(concatName(id), flags);
    }
};

} // namespace GSX
