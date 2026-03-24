#include "stdafx.h"

#include "MemPatch.h"

#include "Context.h"
#include <MinHook.h>

void MainContext::EnableAutoFix()
{
	std::string exe_name = ModuleNameA(NULL);
	//workaround for template deduction after including <locale>
	std::transform(exe_name.begin(), exe_name.end(), exe_name.begin(), [&](int i) { return std::tolower(i); });

	if (exe_name == "ffxiiiimg.exe")
	{
		autofix = AutoFixes::FINAL_FANTASY_XIII;
		spdlog::info("AutoFix for \"Final Fantasy XIII\" enabled");
		FF13_InitializeGameAddresses();
		FF13_HandleLargeAddressAwarePatch();
	}
}

HANDLE WINAPI MainContext::HookCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	const char* ffxiiiimgPrt = strstr(lpFileName, "ffxiiiimg.exe");
	if (ffxiiiimgPrt) {
		spdlog::info("HookCreateFileA Before Replacement: {}", lpFileName);

		int arrayPosition = ffxiiiimgPrt - lpFileName;
		int len = strlen(lpFileName);
		char* newFileName = new char[len + 1];
		strcpy_s(newFileName, len + 1, lpFileName);
		const char* untouched = "untouched"; // needs to have the size of "ffxiiiimg"

		memcpy(newFileName + arrayPosition, untouched, strlen(untouched));
		spdlog::info("HookCreateFileA After Replacement: {}", newFileName);
		MH_STATUS disableHookCreateFileA = MH_DisableHook(CreateFileA);
		spdlog::info("disableHookCreateFileA = {}", disableHookCreateFileA);
		MH_STATUS disableHookCreateFileW = MH_DisableHook(CreateFileW);
		spdlog::info("disableHookCreateFileW = {}", disableHookCreateFileW);
		if (GetFileAttributesA(newFileName) == INVALID_FILE_ATTRIBUTES) {
			spdlog::info("ERROR: Unable to get attributes of {}. Does the file exist? Using the regular ffxiiiimg.exe", newFileName);
			strcpy_s(newFileName, len + 1, lpFileName);
		}
		HANDLE fileHandle = context.TrueCreateFileA(newFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);;
		spdlog::info("Returning File Handle for {}", newFileName);
		delete[] newFileName;
		return fileHandle;
	}
	else {
		return context.TrueCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
}

HANDLE WINAPI MainContext::HookCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	const wchar_t* ffxiiiimgPrt = wcsstr(lpFileName, L"ffxiiiimg.exe");;

	if (ffxiiiimgPrt) {
		spdlog::info(L"HookCreateFileW Before Replacement: {}", lpFileName);

		int arrayPosition = ffxiiiimgPrt - lpFileName;
		int len = wcslen(lpFileName);
		wchar_t* newFileName = new wchar_t[len + 1];
		wcscpy_s(newFileName, len + 1, lpFileName);
		const wchar_t* untouched = L"untouched"; //needs to have the size of L"ffxiiiimg"
		wmemcpy(newFileName + arrayPosition, untouched, wcslen(untouched));
		spdlog::info(L"HookCreateFileW After Replacement: {}", newFileName);
		MH_STATUS disableHookCreateFileA = MH_DisableHook(CreateFileA);
		spdlog::info("disableHookCreateFileA = {}", disableHookCreateFileA);
		MH_STATUS disableHookCreateFileW = MH_DisableHook(CreateFileW);
		spdlog::info("disableHookCreateFileW = {}", disableHookCreateFileW);
		if (GetFileAttributesW(newFileName) == INVALID_FILE_ATTRIBUTES) {
			spdlog::info(L"ERROR: Unable to get attributes of {}. Does the file exist?", newFileName);
			wcscpy_s(newFileName, len + 1, lpFileName);
		}
		HANDLE fileHandle = context.TrueCreateFileW(newFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);;
		spdlog::info(L"Returning File Handle {}", newFileName);
		delete[] newFileName;
		return fileHandle;
	}
	else {
		return context.TrueCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
}

void MainContext::FF13_InitializeGameAddresses()
{
	uint8_t* baseAddr = (uint8_t*)GetModuleHandle(NULL);
	spdlog::info("Base Addr = {}", (void*)baseAddr);

	ff13_exe_large_address_aware_flag_address = baseAddr + 0x126;
	ff13_exe_checksum_address = (uint32_t*)(baseAddr + 0x168);
}

void MainContext::FF13_HandleLargeAddressAwarePatch() {
	const uint8_t laaMask = 0x20;
	if (*ff13_exe_large_address_aware_flag_address & laaMask) {
		spdlog::info("LargeAddressAwarePatch found. ff13_exe_large_address_aware_flag = {:X}; ff13_exe_checksum = {:X}", *ff13_exe_large_address_aware_flag_address, *ff13_exe_checksum_address);

		const MH_STATUS createHookCreateFileA = MH_CreateHook(CreateFileA, HookCreateFileA, reinterpret_cast<void**>(&TrueCreateFileA));
		spdlog::info("createHookCreateFileA = {}", createHookCreateFileA);
		const MH_STATUS enableHookCreateFileA = MH_EnableHook(CreateFileA);
		spdlog::info("enableHookCreateFileA = {}", enableHookCreateFileA);

		const MH_STATUS createHookCreateFileW = MH_CreateHook(CreateFileW, HookCreateFileW, reinterpret_cast<void**>(&TrueCreateFileW));
		spdlog::info("createHookCreateFileW = {}", createHookCreateFileW);
		const MH_STATUS enableHookCreateFileW = MH_EnableHook(CreateFileW);
		spdlog::info("enableHookCreateFileW = {}", enableHookCreateFileW);

		uint8_t new_ff13_exe_large_address_aware_flag = *ff13_exe_large_address_aware_flag_address & ~laaMask;
		MemPatch::Patch(ff13_exe_large_address_aware_flag_address, &new_ff13_exe_large_address_aware_flag, 1);
		spdlog::info("LargeAddressAware patched back. ff13_exe_large_address_aware_flag = {:X};", *ff13_exe_large_address_aware_flag_address);

		uint32_t new_ff13_exe_checksum = 0;
		MemPatch::Patch(ff13_exe_checksum_address, &new_ff13_exe_checksum, sizeof(uint32_t));
		spdlog::info("Checksum patched back. ff13_exe_checksum = {:X}", *ff13_exe_checksum_address);

		spdlog::info("LargeAddressAwarePatch handled");
	}
	else {
		spdlog::info("LargeAddressAwarePatch not found.");
	}
}
