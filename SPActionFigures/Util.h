#pragma once
#include <string>
#include <ShlObj.h>
#include "ScriptHookV/inc/types.h"
#include "ScriptHookV/inc/natives.h"
#include "fmt/format.h"
#include "Header.h"

namespace Util
{
	void LoadModel(Hash modelHash);
	void Notify(const std::string& message);
	void NotifyWithPicture(const std::string& senderGXT, const std::string& notifPic, const std::string& messageGXT);
	void DisplayHelpTextThisFrame(const std::string& message);
	void DisplayHelpTextTimed(const std::string& helpTextGXT, int timeMs);
	void GiveMoney(int amount);
	fs::path GetModDataPath(const std::string& dirName);
}