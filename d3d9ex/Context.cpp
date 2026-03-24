#include "stdafx.h"

#include <thread>
#include <array>

#include "MinHook.h"
#include "SimpleIni.h"

#include "IDirect3D9.h"
#include "IDirect3DDevice9.h"
#include "Context.h"
#include "Wrapper.h"

MainContext context;

Config::Config()
{
	std::string inifile = FullPathFromPath(inifilename);

	CSimpleIniW ini;
	ini.LoadFile(inifile.c_str());

	u32 config_version = ini.GetLongValue(L"Version", L"Config");
	if (config_version != CONFIG_VERSION)
	{
		// save file and reload
		ini.Reset();

#define SETTING(_type, _func, _var, _section, _defaultval, _comment) \
	ini.Set##_func(L#_section, L#_var, _defaultval, _comment)
#include "Settings.h"
#undef SETTING

		ini.SetLongValue(L"Version", L"Config", CONFIG_VERSION);
		ini.SaveFile(inifile.c_str());
		ini.Reset();
		ini.LoadFile(inifile.c_str());
	}

#define SETTING(_type, _func, _var, _section, _defaultval, _comment) \
	_var = ini.Get##_func(L#_section, L#_var)
#include "Settings.h"
#undef SETTING
}

MainContext::MainContext()
{
	std::string logfilename = FullPathFromPath("FF13Fix4GB.log");
	auto file_logger = spdlog::basic_logger_mt("file_logger", logfilename, true);
	spdlog::set_default_logger(file_logger);

	//[2014-10-31 23:46:59.678] [thread] [level] Some message
	spdlog::set_pattern("[%Y-%m-%d %T.%e] [%6t] [%l] %v");
	spdlog::set_level(static_cast<spdlog::level::level_enum>(config.GetLogLogLevel()));
	spdlog::flush_on(spdlog::level::err);

	if(config.GetLogLogFlush())
		spdlog::flush_on(spdlog::level::trace);

	spdlog::info("FF13Fix-4GB");

	spdlog::info("Enabling hooks:");
	const MH_STATUS initializeHooks = MH_Initialize();
	spdlog::info("initializeHooks = {}", initializeHooks);

	const MH_STATUS createHookDirect3DCreate9 = MH_CreateHook(D3D9DLL::Get().Direct3DCreate9, HookDirect3DCreate9, reinterpret_cast<void**>(&TrueDirect3DCreate9));
	spdlog::info("createHookDirect3DCreate9 = {}", createHookDirect3DCreate9);
	const MH_STATUS enableHookDirect3DCreate9 = MH_EnableHook(D3D9DLL::Get().Direct3DCreate9);
	spdlog::info("enableHookDirect3DCreate9 = {}", enableHookDirect3DCreate9);

	const MH_STATUS createHookDirect3DCreate9Ex = MH_CreateHook(D3D9DLL::Get().Direct3DCreate9Ex, HookDirect3DCreate9Ex, reinterpret_cast<void**>(&TrueDirect3DCreate9Ex));
	spdlog::info("createHookDirect3DCreate9Ex = {}", createHookDirect3DCreate9Ex);
	const MH_STATUS enableHookDirect3DCreate9Ex = MH_EnableHook(D3D9DLL::Get().Direct3DCreate9Ex);
	spdlog::info("enableHookDirect3DCreate9Ex = {}", enableHookDirect3DCreate9Ex);

	if (config.GetOptionsAutoFix()) EnableAutoFix();
}

MainContext::~MainContext()
{
}

IDirect3D9* WINAPI MainContext::HookDirect3DCreate9(UINT SDKVersion)
{
	spdlog::info(__FUNCTION__);
	IDirect3D9* d3d9 = context.TrueDirect3DCreate9(SDKVersion);
	if (d3d9) {
		return new hkIDirect3D9(static_cast<IDirect3D9Ex*>(d3d9));
	}

	return d3d9;
}

HRESULT WINAPI MainContext::HookDirect3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppIDirect3D9Ex)
{
	spdlog::info(__FUNCTION__);
	HRESULT hr = context.TrueDirect3DCreate9Ex(SDKVersion, ppIDirect3D9Ex);
	if (SUCCEEDED(hr)) {
		hkIDirect3D9* pIDirect3D9Ex = new hkIDirect3D9(*ppIDirect3D9Ex);
		*ppIDirect3D9Ex = pIDirect3D9Ex;
	}

	return hr;
}
