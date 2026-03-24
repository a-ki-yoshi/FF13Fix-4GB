#pragma once

#include <atomic>
#include <d3d9.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "WinUtil.h"

#include "Types.h"

static const char* inifilename = "FF13Fix4GB.ini";
#define CONFIG_VERSION 7

class Config
{
public:
	Config(const Config&) = delete;
	const Config& operator=(Config& other) = delete;

	Config();

#define SETTING(_type, _func, _var, _section, _defaultval, _comment) \
	private: _type _var; \
	public: const _type& Get##_section##_var() const { return _var; };
#include "Settings.h"
#undef SETTING
};

#define DECLARE_HOOK(type, callconv, name, ...) \
	public: type(callconv* True##name)(__VA_ARGS__) = name; \
	private: static type callconv Hook##name(__VA_ARGS__);

class MainContext
{
	MainContext(const MainContext&) = delete;
	const MainContext& operator=(MainContext& other) = delete;

	DECLARE_HOOK(IDirect3D9*, WINAPI, Direct3DCreate9, UINT SDKVersion);
	DECLARE_HOOK(HRESULT, WINAPI, Direct3DCreate9Ex, UINT SDKVersion, IDirect3D9Ex** ppIDirect3D9Ex);

	DECLARE_HOOK(HANDLE, WINAPI, CreateFileA, LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	DECLARE_HOOK(HANDLE, WINAPI, CreateFileW, LPCWSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);

public:
	MainContext();
	virtual ~MainContext();

	Config config;

private:
	enum class AutoFixes : u32
	{
		NONE = 0,
		FINAL_FANTASY_XIII,
	};

	void EnableAutoFix();

	AutoFixes autofix = AutoFixes::NONE;

	uint8_t* ff13_exe_large_address_aware_flag_address = NULL;
	uint32_t* ff13_exe_checksum_address = NULL;

	void FF13_InitializeGameAddresses();
	void FF13_HandleLargeAddressAwarePatch();
};

extern MainContext context;
