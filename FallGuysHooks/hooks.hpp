#pragma once
#include <Windows.h>
#include <string>
#include <sstream>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <any>
#include <type_traits>
#include <utility>
#include <mutex>
#include <easyhook.h>
#include "wrapper.hpp"

namespace hook::types
{
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

    using set_username_t =  int(__fastcall*)(void* self, string* username);
    using set_nickname_t = int(__fastcall*)(void* self, string* nickname);
    using set_displayname_t = int(__fastcall*)(void* self, string* displayname);
    using get_displayname_t = int(__fastcall*)(void* self, net_obj* obj);
    using debug_make_player_fast_t = void*(__fastcall*)(void* self);
    using update_t = void*(__fastcall*)(void* self);
    using get_my_player_t = void*(__fastcall*)(void* self);
    using fixed_update_all_t = int(__fastcall*)(void* self);
    using log_t = void(__stdcall*)(string* message, void* method);
    using stun_t = int(__fastcall*)(void* self);
    using get_can_jump_t = bool(__fastcall*)(void* self);
    using debug_set_zero_gravity_t = void(__fastcall*)(void* self, bool enabled);
}

using namespace hook::types;

class hooks
{
private:
    static inline std::unordered_map<void*, std::pair<std::string, void*>> m_hooks = {};

    template<class T = void>
    static void log(std::vector<std::any>& args, std::string& name, std::any ret = nullptr)
    {
        std::cout << name << "(";
        for (auto i = 0; i < args.size(); ++i)
        {
            auto& arg = args[i];

            if (arg.type() == typeid(string*))
            {
                wprintf(L"%ls", std::any_cast<string*>(arg)->value);
                //std::wcout << std::any_cast<string*>(arg)->value;
            }
            else if (arg.type() == typeid(void*))
            {
                std::cout << std::any_cast<void*>(arg);
            }
            else
            {
                std::cout << std::any_cast<int>(arg);
            }

            if (i != args.size() - 1)
            {
                std::cout << ", ";
            }
        }

        std::cout << ")";

        if (!std::is_same<T, void>::value)
        {
            std::cout << " -> ";

            if (ret.type() == typeid(void*))
            {
                std::cout << std::any_cast<void*>(ret);
            }
            else
            {
                std::cout << std::any_cast<int>(ret);
            }
        }

        std::cout << std::endl;
    }

    template<class T>
    static T get_original(void* hook)
    {
        return reinterpret_cast<T>(m_hooks[hook].second);
    }

    template<class T, class U, typename... Args>
    static T call_original(void* hook, Args&&... args)
    {
        auto& [name, ptr] = m_hooks[hook];
        auto fn = reinterpret_cast<U>(ptr);
        auto ret = fn(std::forward<Args>(args)...);
        auto value = static_cast<T>(ret);

        std::vector<std::any> arg_list = { args... };
        log<T>(arg_list, name, ret);
        
        return ret;
    }

    template<class U, typename... Args>
    static void call_original(void* hook, Args&&... args)
    {
        auto& [name, ptr] = m_hooks[hook];
        auto fn = reinterpret_cast<U>(ptr);
        fn(std::forward<Args>(args)...);

        std::vector<std::any> arg_list = { args... };
        log(arg_list, name);
    }

public:
    template<class T>
    static bool add_hook(void* base, int offset, void* hook, std::string name)
    {
        auto addr = reinterpret_cast<uint64_t>(base) + offset;
        auto ptr = reinterpret_cast<void*>(addr);
        m_hooks[hook] = { name, ptr };

        std::cout << "[HOOKS] " << name << " @ " << ptr << std::endl;

        return install_hook(ptr, hook);
    }

    template<class T>
    static bool add_hook(void* base, int offset, void* hook)
    {
        auto addr = reinterpret_cast<uint64_t>(base) + offset;
        auto ptr = reinterpret_cast<void*>(addr);
        m_hooks[hook] = { "", ptr };

        return install_hook(ptr, hook);
    }

    static bool install_hook(void* ptr, void* hook)
    {
        HOOK_TRACE_INFO hHook = { NULL };
        NTSTATUS result = LhInstallHook(
            ptr,
            hook,
            NULL,
            &hHook);
        if (FAILED(result))
        {
            std::wstring error = RtlGetLastErrorString();
            return false;
        }
        else
        {
            ULONG ACLEntries[1] = { 0 };
            LhSetExclusiveACL(ACLEntries, 1, &hHook);
            return true;
        }
    }

    static int __fastcall set_username_hook(void* self, string* username)
    {
        username->value[0] = 'B';
            
        return get_original<set_username_t>(set_username_hook)(self, username);
    }

    static int __fastcall set_nickname_hook(void* self, string* nickname)
    {
        nickname->value[0] = 'B';
            
        return get_original<set_nickname_t>(set_nickname_hook)(self, nickname);
    }

    static int __fastcall set_displayname_hook(void* self, string* displayname)
    {
        displayname->value[0] = 'B';

        return get_original<set_displayname_t>(set_displayname_hook)(self, displayname);
    }

    static int __fastcall get_displayname_hook(void* self, net_obj* netobj)
    {
        return get_original<get_displayname_t>(get_displayname_hook)(self, netobj);
    }

    static void* __fastcall debug_make_player_fast_hook(void* self)
    {
        printf("fasthook\n");
        return get_original<debug_make_player_fast_t>(debug_make_player_fast_hook)(self);
    }

    static void* __fastcall get_my_player_hook(void* self)
    {
        return get_original<get_my_player_t>(get_my_player_hook)(self);
    }

    static int __fastcall fixed_update_all_hook(void* self)
    {
        return get_original<fixed_update_all_t>(fixed_update_all_hook)(self);
    }

    static inline bool hacked = false;
    static void* __fastcall update_hook(void* self)
    {
        if (!hacked)
        {
            if (get_original<get_my_player_t>(get_my_player_hook)(self) != nullptr) // speed
            {
                get_original<debug_make_player_fast_t>(debug_make_player_fast_hook)(self);
                hacked = true;
            }
        }

        return get_original<update_t>(update_hook)(self);
    }

    static void __stdcall log_hook(string* message, void* method)
    {
        get_original<log_t>(log_hook)(message, method);
    }

    static int __fastcall stun_hook(void* self)
    {
        printf("Stun(%p)\n", self);
        return 0;
    }

    static bool __fastcall get_can_jump_hook(void* self)
    {
        printf("GetCanJump(%p)\n", self);
        return true;
    }

    static void __fastcall debug_set_zero_gravity_hook(void* self, bool enabled)
    {
        get_original<debug_set_zero_gravity_t>(debug_set_zero_gravity_hook)(self, enabled);
    }
};