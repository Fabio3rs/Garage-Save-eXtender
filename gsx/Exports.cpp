#include <cstdint>

extern "C" __declspec(dllexport) const char *__cdecl GSX_getCompileTime() {
    return __DATE__ " " __TIME__;
}

extern "C" __declspec(dllexport) const char *__cdecl GSX_getVersionString() {
    return "0.4.9a";
}

extern "C" __declspec(dllexport) float __cdecl GSX_getVersionNum() {
    return 0.4905F;
}

extern "C" __declspec(dllexport) int32_t __cdecl GSX_getVersionNumInt32() {
    return 0x00040905;
}
