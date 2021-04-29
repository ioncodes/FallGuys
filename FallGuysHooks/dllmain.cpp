#include <iostream>
#include <string>
#include "wrapper.hpp"
#include <Windows.h>
#include <easyhook.h>
#include "hooks.hpp"

using namespace hook::types;

extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO * remote_info);

void allocate_console()
{
	winapi::process::alloc_console();
	//freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen("debug.log", "w", stdout);
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

void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* remote_info)
{
	allocate_console();
	
	auto game_assembly = find_module(L"GameAssembly.dll");
	std::cout << "[HOOKS] GameAssembly.dll @ " << game_assembly << std::endl;
	
	hooks::add_hook<set_username_t>(game_assembly, 0x175bd80, hooks::set_username_hook, "PartyNameTag::SetUsername");
	hooks::add_hook<set_nickname_t>(game_assembly, 0x175baa0, hooks::set_nickname_hook, "PartyNameTag::SetNickname");
	hooks::add_hook<set_displayname_t>(game_assembly, 0x17d67d0, hooks::set_displayname_hook, "PlayerProfile::SetDisplayname");
	hooks::add_hook<get_displayname_t>(game_assembly, 0x1742220, hooks::get_displayname_hook, "ClientGameManager::GetDisplayNameForNetObjID");
	hooks::add_hook<fixed_update_all_t>(game_assembly, 0x07132a0, hooks::fixed_update_all_hook, "MPGNetObjectManager::FixedUpdateAll");
	hooks::add_hook<log_t>(game_assembly, 0x1b4b360, hooks::log_hook, "Debug::Log");
	hooks::add_hook<debug_make_player_fast_t>(game_assembly, 0x1740cf0, hooks::debug_make_player_fast_hook, "ClientGameManager::DebugMakePlayerFast");
	hooks::add_hook<update_t>(game_assembly, 0x1742400, hooks::update_hook, "ClientGameManager::Update");
	hooks::add_hook<get_my_player_t>(game_assembly, 0x1742400, hooks::get_my_player_hook, "ClientGameManager::GetMyPlayer");
	hooks::add_hook<debug_set_zero_gravity_t>(game_assembly, 0x1742400, hooks::debug_set_zero_gravity_hook, "ClientGameManager::DebugSetZeroGravity");
	hooks::add_hook<stun_t>(game_assembly, 0x1796030, hooks::stun_hook, "FallGuysCharacterController::Stun");
	hooks::add_hook<get_can_jump_t>(game_assembly, 0x17978f0, hooks::get_can_jump_hook, "FallGuysCharacterController::GetCanJump");
}