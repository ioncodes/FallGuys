#include <iostream>
#include <string>
#include "wrapper.hpp"
#include <Windows.h>
#include <easyhook.h>
#include "hooks.hpp"

extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO * remote_info);

bool hook(void* original, void* hook)
{
	HOOK_TRACE_INFO hHook = { NULL };
	NTSTATUS result = LhInstallHook(
		original,
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

void allocate_console()
{
	winapi::process::alloc_console();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
}

void* find_module(std::wstring target_module)
{
	auto process = winapi::process::get_current_process();
	auto modules = winapi::module::enum_process_modules(process);

	for (auto module : modules)
	{
		auto module_name = winapi::module::get_module_file_name_ex(
			process,
			module);

		if (module_name.find(target_module) != std::string::npos)
		{
			return module;
		}
	}

	return nullptr;
}

void hook_pnt_set_username(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] PartyNameTag::SetUsername @ " << ptr << std::endl;
	hooks::pnt::original_set_username = reinterpret_cast<
		hooks::types::pnt_set_username_t>(ptr);
	hook(ptr, hooks::pnt::set_username_hook);
}

void hook_pnt_set_nickname(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] PartyNameTag::SetNickname @ " << ptr << std::endl;
	hooks::pnt::original_set_nickname = reinterpret_cast<
		hooks::types::pnt_set_nickname_t>(ptr);
	hook(ptr, hooks::pnt::set_nickname_hook);
}

void hook_pp_set_displayname(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] PlayerProfile::SetDisplayname @ " << ptr << std::endl;
	hooks::pp::original_set_displayname = reinterpret_cast<
		hooks::types::pp_set_displayname_t>(ptr);
	hook(ptr, hooks::pp::set_displayname_hook);
}

void hook3(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] ClientGameManager::GetDisplayNameForNetObjID @ " << ptr << std::endl;
	hooks::cgm::original_get_displayname = reinterpret_cast<
		hooks::types::cgm_get_displayname_for_net_obj_id_t>(ptr);
	hook(ptr, hooks::cgm::get_displayname_hook);
}

void hook4(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] MPGNetObjectManager::FixedUpdateAll @ " << ptr << std::endl;
	hooks::obj_manager::original_fixed_update_all = reinterpret_cast<
		hooks::types::obj_manager_fixed_update_all>(ptr);
	hook(ptr, hooks::obj_manager::fixed_update_all_hook);
}

void hook5(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] Debug::Log @ " << ptr << std::endl;
	hooks::dbg::original_log = reinterpret_cast<
		hooks::types::dbg_log_t>(ptr);
	hook(ptr, hooks::dbg::log_hook);
}

void hook6(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] ClientGameManager::DebugMakePlayerFast @ " << ptr << std::endl;
	hooks::cgm::original_debug_make_player_fast = reinterpret_cast<
		hooks::types::cgm_debug_make_player_fast_t>(ptr);
	hook(ptr, hooks::cgm::debug_make_player_fast_hook);
}

void hook7(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] ClientGameManager::Update @ " << ptr << std::endl;
	hooks::cgm::original_update = reinterpret_cast<
		hooks::types::cgm_update_t>(ptr);
	hook(ptr, hooks::cgm::update_hook);
}

void hook8(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] ClientGameManager::GetMyPlayer @ " << ptr << std::endl;
	hooks::cgm::original_get_my_player = reinterpret_cast<
		hooks::types::cgm_get_my_player_t>(ptr);
	hook(ptr, hooks::cgm::get_my_player_hook);
}

void hook9(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] ClientGameManager::DebugSetZeroGravity @ " << ptr << std::endl;
	hooks::fgcc::original_debug_set_zero_gravity = reinterpret_cast<
		hooks::types::fgcc_debug_set_zero_gravity_t>(ptr);
	hook(ptr, hooks::fgcc::debug_set_zero_gravity_hook);
}

void hook10(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] FallGuysCharacterController::Stun @ " << ptr << std::endl;
	hooks::fgcc::original_stun = reinterpret_cast<
		hooks::types::fgcc_stun_t>(ptr);
	hook(ptr, hooks::fgcc::stun_hook);
}

void hook11(void* game_assembly, int offset)
{
	auto addr = reinterpret_cast<uint64_t>(game_assembly) + offset;
	auto ptr = reinterpret_cast<void*>(addr);
	std::cout << "[HOOKS] FallGuysCharacterController::GetCanJump @ " << ptr << std::endl;
	hooks::fgcc::original_get_can_jump = reinterpret_cast<
		hooks::types::fgcc_get_can_jump_t>(ptr);
	hook(ptr, hooks::fgcc::get_can_jump_hook);
}

void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* remote_info)
{
	allocate_console();
	
	auto game_assembly = find_module(L"GameAssembly.dll");
	std::cout << "[HOOKS] GameAssembly.dll @ " << game_assembly << std::endl;
	
	hook_pnt_set_username(game_assembly, 0x175bd80);
	hook_pnt_set_nickname(game_assembly, 0x175baa0);
	hook_pp_set_displayname(game_assembly, 0x17d67d0);
	hook3(game_assembly, 0x1742220);
	hook4(game_assembly, 0x7132a0);
	hook5(game_assembly, 0x1b4b360);
	hook6(game_assembly, 0x1740cf0);
	hook7(game_assembly, 0x17483f0);
	hook8(game_assembly, 0x1742400);
	hook9(game_assembly, 0x1798720);
	hook10(game_assembly, 0x1796030);
	hook11(game_assembly, 0x17978f0);
}