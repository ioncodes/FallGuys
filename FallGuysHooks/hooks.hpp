#pragma once
#include <Windows.h>
#include <string>
#include <sstream>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <vector>
#include "wrapper.hpp"

namespace hooks
{
    class types
    {
    public:
        struct string
        {
            void* self;
            uint64_t idk0;
            uint32_t length;
            wchar_t value[0];
        };
        struct net_obj
        {
            uint32_t network_id;
        };
        using pnt_set_username_t = int(__fastcall*)(void* self, string* username);
        using pnt_set_nickname_t = int(__fastcall*)(void* self, string* nickname);
        using pp_set_displayname_t = int(__fastcall*)(void* self, string* displayname);
        using cgm_get_displayname_for_net_obj_id_t = int(__fastcall*)(void* self, net_obj* obj);
        using cgm_debug_make_player_fast_t = int(__fastcall*)(void* self);
        using cgm_update_t = int(__fastcall*)(void* self);
        using cgm_get_my_player_t = void*(__fastcall*)(void* self);
        using obj_manager_fixed_update_all = int(__fastcall*)(void* self);
        using dbg_log_t = void(__stdcall*)(string* message, void* method);
        using fgcc_stun_t = int(__fastcall*)(void* self);
        using fgcc_get_can_jump_t = bool(__fastcall*)(void* self);
        using fgcc_debug_set_zero_gravity_t = void(__fastcall*)(void* self, bool enabled);
    };

    class pnt
    {
    public:
        static inline types::pnt_set_username_t original_set_username = nullptr;
        static int __fastcall set_username_hook(
            void* self, types::string* username)
        {
            username->value[0] = 'B';
            
            wprintf(L"SetUsername(%p, %ls)\n", self, username->value);
            return hooks::pnt::original_set_username(self, username);
        }

        static inline types::pnt_set_nickname_t original_set_nickname = nullptr;
        static int __fastcall set_nickname_hook(
            void* self, types::string* nickname)
        {
            nickname->value[0] = 'B';
            
            wprintf(L"SetNickname(%p, %ls)\n", self, nickname->value);
            return hooks::pnt::original_set_nickname(self, nickname);
        }
    };

    class pp
    {
    public:
        static inline types::pp_set_displayname_t original_set_displayname = nullptr;
        static int __fastcall set_displayname_hook(
            void* self, types::string* displayname)
        {
            displayname->value[0] = 'B';

            wprintf(L"SetDisplayname(%p, %ls)\n", self, displayname->value);
            return hooks::pp::original_set_displayname(self, displayname);
        }
    };

    class cgm
    {
    public:
        static inline types::cgm_get_displayname_for_net_obj_id_t original_get_displayname = nullptr;
        static int __fastcall get_displayname_hook(void* self, types::net_obj* netobj)
        {
            printf("GetDisplayname(%p, %d)\n", self, netobj->network_id);
            return hooks::cgm::original_get_displayname(self, netobj);
        }

        static inline types::cgm_debug_make_player_fast_t original_debug_make_player_fast = nullptr;
        static int __fastcall debug_make_player_fast_hook(void* self)
        {
            printf("DebugMakePlayerFast(%p)\n", self);
            return hooks::cgm::original_debug_make_player_fast(self);
        }

        static inline bool hacked = false;

        static inline types::cgm_update_t original_update = nullptr;
        static int __fastcall update_hook(void* self)
        {
            //printf("Update(%p)\n");
            if (!hacked)
            {
                if (original_get_my_player(self) != nullptr)
                {
                    original_debug_make_player_fast(self);
                    //hooks::fgcc::original_debug_set_zero_gravity(self, true);
                    hacked = true;
                }
            }

            return hooks::cgm::original_update(self);
        }

        static inline types::cgm_get_my_player_t original_get_my_player = nullptr;
        static void* __fastcall get_my_player_hook(void* self)
        {
            void* ret = original_get_my_player(self);
            if(ret != nullptr)
                printf("GetMyPlayer(%p) -> %p\n", self, ret);
            return ret;
        }
    };

    class obj_manager
    {
    public:
        static inline types::obj_manager_fixed_update_all original_fixed_update_all = nullptr;
        static int __fastcall fixed_update_all_hook(void* self)
        {
            printf("FixedUpdateAll(%p)\n", self);
            return hooks::obj_manager::original_fixed_update_all(self);
        }
    };

    class dbg
    {
    public:
        static inline types::dbg_log_t original_log = nullptr;
        static void __stdcall log_hook(types::string* message, void* method)
        {
            //wprintf(L"Log(%ls, %p)\n", message->value, method);
            return original_log(message, method);
        }
    };

    class fgcc
    {
    public:
        static inline types::fgcc_stun_t original_stun = nullptr;
        static int __fastcall stun_hook(void* self)
        {
            printf("Stun(%p)\n", self);
            return 0;
        }

        static inline types::fgcc_get_can_jump_t original_get_can_jump = nullptr;
        static bool __fastcall get_can_jump_hook(void* self)
        {
            printf("GetCanJump(%p)\n", self);
            return true;
        }

        static inline types::fgcc_debug_set_zero_gravity_t original_debug_set_zero_gravity = nullptr;
        static void __fastcall debug_set_zero_gravity_hook(void* self, bool enabled)
        {
            printf("DebugSetZeroGravity(%p, %b)\n", self, enabled);
            original_debug_set_zero_gravity(self, enabled);
        }
    };
};