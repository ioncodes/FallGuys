#include <Windows.h>
#include <iostream>
#include <string>
#include <easyhook.h>
#include "wrapper.hpp"

bool inject_dll(int process_id, std::wstring payload)
{
	NTSTATUS status = RhInjectLibrary(
		process_id,
		0,
		EASYHOOK_INJECT_DEFAULT,
		NULL,
		(wchar_t*)payload.data(),
		NULL,
		0);

	if (status != 0)
	{
		std::wcout << L"Injection into " << process_id << " failed with error code = " << status << std::endl;
		PWCHAR error = RtlGetLastErrorString();
		std::wcout << error << std::endl;
	}
	else
	{
		std::wcout << L"Library injected into " << process_id << " successfully." << std::endl;
	}

	return SUCCEEDED(status);
}

uint32_t find_process_id(std::wstring target_name)
{
	auto proc_info = nt::system::query_system_information<
		nt::system_process_information>();
	for (auto proc : proc_info)
	{
		if (proc->image_name.length > 0)
		{
			auto process_id = reinterpret_cast<std::uint32_t>(
				proc->process_id);
			auto process_name = std::wstring(proc->image_name.buffer);
			if (process_name == target_name)
			{
				return process_id;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	auto process_id = find_process_id(L"FallGuys_client.exe");
	auto success = inject_dll(process_id, L"..\\x64\\Debug\\FallGuysHooks.dll");
	return success;
}