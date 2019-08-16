#include "Util.h"

namespace Util
{
	void LoadModel(Hash modelHash)
	{
		if (STREAMING::HAS_MODEL_LOADED(modelHash))
		{
			return;
		}

		STREAMING::REQUEST_MODEL(modelHash);
		while (!STREAMING::HAS_MODEL_LOADED(modelHash)) WAIT(0);
	}

	void Notify(const std::string& message)
	{
		HUD::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(message.c_str());
		HUD::_DRAW_NOTIFICATION(false, true);
	}

	void NotifyWithPicture(const std::string& senderGXT, const std::string& notifPic, const std::string& messageGXT)
	{
		HUD::_SET_NOTIFICATION_TEXT_ENTRY(messageGXT.c_str());
		HUD::_SET_NOTIFICATION_MESSAGE_2(notifPic.c_str(), notifPic.c_str(), false, 1, HUD::_GET_LABEL_TEXT(senderGXT.c_str()), 0);

		Hash model = ENTITY::GET_ENTITY_MODEL(PLAYER::PLAYER_PED_ID());
		std::string soundSetName = "Phone_SoundSet_Default";

		if (model == MISC::GET_HASH_KEY("player_zero")) {
			soundSetName = "Phone_SoundSet_Michael";
		}
		else if (model == MISC::GET_HASH_KEY("player_one")) {
			soundSetName = "Phone_SoundSet_Franklin";
		}
		else if (model == MISC::GET_HASH_KEY("player_two")) {
			soundSetName = "Phone_SoundSet_Trevor";
		}

		AUDIO::PLAY_SOUND_FRONTEND(-1, "Text_Arrive_Tone", soundSetName.c_str(), true);
	}

	void DisplayHelpTextThisFrame(const std::string& message)
	{
		// Meet Impotent Rage helptext + collection progress helptext = no fun
		HUD::BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED("PBSH_INTERACT");
		if (HUD::END_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(0))
		{
			return;
		}

		HUD::BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(message.c_str());
		HUD::END_TEXT_COMMAND_DISPLAY_HELP(0, false, true, -1);
	}

	void DisplayHelpTextTimed(const std::string& helpTextGXT, int timeMs)
	{
		HUD::BEGIN_TEXT_COMMAND_DISPLAY_HELP(helpTextGXT.c_str());
		HUD::END_TEXT_COMMAND_DISPLAY_HELP(0, false, true, timeMs);
	}

	void GiveMoney(int amount)
	{
		Hash model = ENTITY::GET_ENTITY_MODEL(PLAYER::PLAYER_PED_ID());
		std::string prefix;

		if (model == MISC::GET_HASH_KEY("player_zero")) {
			prefix = "SP0";
		}
		else if (model == MISC::GET_HASH_KEY("player_one")) {
			prefix = "SP1";
		}
		else if (model == MISC::GET_HASH_KEY("player_two")) {
			prefix = "SP2";
		}

		if (!prefix.empty()) {
			Hash moneyStat = MISC::GET_HASH_KEY(fmt::format("{}_TOTAL_CASH", prefix).c_str());
			Hash totalEarnedStat = MISC::GET_HASH_KEY(fmt::format("{}_TOTAL_CASH_EARNED", prefix).c_str());

			int currentMoney, currentTotalEarned;
			STATS::STAT_GET_INT(moneyStat, &currentMoney, -1);
			STATS::STAT_GET_INT(totalEarnedStat, &currentTotalEarned, -1);
			STATS::STAT_SET_INT(moneyStat, currentMoney + amount, true);
			STATS::STAT_SET_INT(totalEarnedStat, currentTotalEarned + amount, true);
		}
	}

	fs::path GetModDataPath(const std::string& dirName)
	{
		fs::path path;
		PWSTR appDataPath = NULL;

		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &appDataPath)))
		{
			path = fs::path(appDataPath).append(dirName);
			CoTaskMemFree(appDataPath);
		}
		else
		{
			path = fs::current_path().append(dirName);
		}

		return path;
	}
}