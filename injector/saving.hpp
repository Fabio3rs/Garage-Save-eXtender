/*
 *  Injectors - Save Notification
 *
 *  Copyright (C) 2012-2014 LINK/2012 <dma_2012@hotmail.com>
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not
 * be misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 *
 */
#pragma once
#include <algorithm>
#include <array>
#include <cstring>
#include <injector/hooking.hpp>
#include <memory>
#include <string_view>

namespace injector {
class save_manager {
  private:
    // Hooks
    using fnew_hook = function_hooker<scoped_call, 0x53E59B, char()>;
    using ldng_hook = function_hooker<scoped_call, 0x53C6EA, void()>;
    using ldngb_hook = function_hooker<scoped_call, 0x53C70B, char(char *)>;
    using onsav_hook =
        function_hooker_fastcall<scoped_call, 0x578DFA, int(void *, int, int)>;

    // Prototypes
    using OnLoadType = std::function<void(int)>;
    using OnSaveType = std::function<void(int)>;

    // Callbacks storage
    static auto OnLoadCallback() -> OnLoadType & {
        static OnLoadType cb;
        return cb;
    }

    static auto OnSaveCallback() -> OnSaveType & {
        static OnSaveType cb;
        return cb;
    }

    // Necessary game vars
    static auto IsLoad() -> bool {
        return ReadMemory<char>(0xBA6748 + 0x60) != 0;
    }
    static auto GetSlot() -> char { return ReadMemory<char>(0xBA6748 + 0x15F); }
    static auto SetDirMyDocuments() -> bool {
        return (memory_pointer(0x538860).get<int()>()()) != 0;
    }

    // Calls on load callback if possible
    static void CallOnLoad(int slot) {
        if (auto &cb = OnLoadCallback()) {
            cb(slot);
        }
    }

    // Calls on save callback if possible
    static void CallOnSave(int slot) {
        if (auto &cb = OnSaveCallback()) {
            cb(slot);
        }
    }

    // Patches the game to notify callbacks
    static void Patch() {
        static bool bPatched = false;
        if (bPatched) {
            return;
        }
        bPatched = true;

        // On the first time the user does a new-game/load-game...
        static auto firstTime =
            make_function_hook<fnew_hook>([](const fnew_hook::func_type &func) {
                if (!IsLoad()) {
                    CallOnLoad(-1);
                }
                return func();
            });

        // On the second time+ a new game happens or whenever a load game
        // happens...
        static auto secondTimePlus =
            make_function_hook<ldng_hook>([](const ldng_hook::func_type &func) {
                if (!IsLoad()) {
                    CallOnLoad(-1);
                }
                return func();
            });

        // Whenever a load game happens
        static auto wheneverLoad = make_function_hook<ldngb_hook>(
            [](const ldngb_hook::func_type &GenericLoad, char *&e) {
                auto result = GenericLoad(e);
                if (result != 0) {
                    CallOnLoad(GetSlot());
                }
                return result;
            });

        // Whenever a save game happens
        static auto saveGame = make_function_hook<onsav_hook>(
            [](const onsav_hook::func_type &GenericSave, void *&self, int &,
               int &savenum) {
                auto result = GenericSave(self, 0, savenum);
                if (result == 0) {
                    CallOnSave(GetSlot());
                }
                return result;
            });
    }

  public:
    // RAII wrapper to SetDirMyDocuments, scoped change to user directory
    struct scoped_userdir {
        std::array<char, MAX_PATH> buffer{};

        scoped_userdir() {
            GetCurrentDirectoryA(buffer.size(), buffer.data());
            SetDirMyDocuments();
        }

        explicit scoped_userdir(std::string_view customPath) {
            GetCurrentDirectoryA(buffer.size(), buffer.data());

            if (customPath.data() != nullptr && !customPath.empty()) {
                SetCurrentDirectoryA(customPath.data());
            }
        }

        static auto blank() -> scoped_userdir {
            std::string_view blankStr;
            return scoped_userdir{blankStr};
        }

        scoped_userdir(scoped_userdir &&other) noexcept {
            if (this == std::addressof(other)) {
                return;
            }

            buffer = other.buffer;
            other.buffer[0] = '\0';
        }

        auto operator=(scoped_userdir &&other) noexcept -> scoped_userdir & {
            if (this == std::addressof(other)) {
                return *this;
            }

            buffer = other.buffer;
            other.buffer[0] = '\0';
            return *this;
        }

        scoped_userdir(const scoped_userdir &other) noexcept = delete;
        auto operator=(const scoped_userdir &other) noexcept
            -> scoped_userdir & = delete;

        ~scoped_userdir() {
            if (strnlen(buffer.data(), buffer.size()) > 0) {
                SetCurrentDirectoryA(buffer.data());
            }
        }
    };

    // Setup a callback to call whenever a new game or load game happens
    static void on_load(const OnLoadType &fn) {
        Patch();
        OnLoadCallback() = fn;
    }

    // Setup a callback to call whenever a save game happens
    static void on_save(const OnSaveType &fn) {
        Patch();
        OnSaveCallback() = fn;
    }
};

} // namespace injector
