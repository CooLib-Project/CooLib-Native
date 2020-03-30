﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "dll.h"
#include <cstdint>
#include "nlohmann/json.hpp"
#include <shlwapi.h>

using json = nlohmann::json;

int32_t AuthCode;
HMODULE _hModule = 0;

tCQ_addLog CQ_addLog;
tAdd Add;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		_hModule = hModule;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" const char* __stdcall AppInfo()
{
	return "9,cn.coorg.coolib.testapp";
}

extern "C" int32_t __stdcall Initialize(int32_t _AuthCode)
{
	AuthCode = _AuthCode;
	return 0;
}

extern "C" int32_t __stdcall _eventStartup()
{
	HMODULE h;
	h = GetModuleHandleA("CQP.dll");
	CQ_addLog = reinterpret_cast<tCQ_addLog>(GetProcAddress(h, "CQ_addLog"));

	// 通知 CooLib
	typedef bool(__stdcall* tLibLoaded)(const char* a);
	char dllPath[MAX_PATH];
	if (GetModuleFileNameA(_hModule, dllPath, MAX_PATH) == 0) return 0;
	PathRemoveFileSpecA(dllPath);
	char szFilePath[MAX_PATH];
	lstrcpyA(szFilePath, dllPath);
	lstrcatA(szFilePath, "\\cn.coorg.coolib.cpk");
	HMODULE hcl = LoadLibraryA(szFilePath);
	if (hcl == 0) {
		lstrcpyA(szFilePath, dllPath);
		lstrcatA(szFilePath, "\\cn.coorg.coolib.dll");
		hcl = LoadLibraryA(szFilePath);
		if (hcl == 0) return 0;
	}
	tLibLoaded LibLoaded;
	DWORD l;
	LibLoaded = reinterpret_cast<tLibLoaded>(GetProcAddress(hcl, "LibLoaded"));
	if (LibLoaded) {
		LibLoaded("cn.coorg.coolib.testapp");
	}
	// 请保持 Coolib 加载，*不要*调用 FreeLibrary

	return 0;
}

extern "C" int32_t __stdcall _eventEnable() {
	return 0;
}

extern "C" int32_t __stdcall _eventDisable() {

	return 0;
}

extern "C" int32_t __stdcall _eventExit()
{

	return 0;
}

extern "C" const char* __stdcall LibInfo() {
	return (const char*)R"(
{
    "ver": 1,
    "AppID": "cn.coorg.coolib.testapp",
    "AppVer": "0.0.1",
    "require": {
        "cn.coorg.coolib.testlib": "*"
    },
    "using": {
        "Add": "cn.coorg.coolib.testlib::Add"
    },
    "LibAPI": {}
}
)";
}

extern "C" int32_t __stdcall LibCallback(bool a, const char* b) {
	if (a) {
		json j = json::parse(b);
		Add = reinterpret_cast<tAdd>(j["FuncAddr"]["Add"].get<int32_t>());
	}
	if (CQ_addLog) {
		CQ_addLog(AuthCode, 0, "CooLib-TestApp", "AuthCode OK.");
	}
	return 0;
}

extern "C" int32_t __stdcall AppCallback() {
	if (Add != nullptr) {
		CQ_addLog(AuthCode, 0, "CooLib-TestApp", std::to_string(Add((int32_t)2, (int32_t)3)).c_str());
	}
	return 0;
}

extern "C" int32_t __stdcall DisableCallback() {
	return 0;
}

extern "C" int32_t __stdcall ExitCallback(const char* a) {
	return 0;
}
