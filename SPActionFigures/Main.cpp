#include "ScriptHookV/inc/main.h"
#include "Script.h"
#include "Logger.h"

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
			ScriptLog.Clear();
			ScriptLog.Write(LogLevel::LOG_INFO, "Script started");

			scriptRegister(hInstance, ScriptInit);
			break;

		case DLL_PROCESS_DETACH:
			ScriptLog.Write(LogLevel::LOG_INFO, "Script stopped");
			scriptUnregister(hInstance);
			break;
	}

	return TRUE;
}