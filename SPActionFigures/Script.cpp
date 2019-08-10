#include "ScriptHookV/inc/types.h"
#include "ScriptHookV/inc/natives.h"
#include "pugixml/pugixml.hpp"
#include "simpleini/SimpleIni.h"
#include "fmt/format.h"
#include "Header.h"
#include "Script.h"
#include "Constants.h"
#include "Logger.h"
#include "Figure.h"
#include "Util.h"

DWORD g_currentTick;
DWORD g_nextUpdateTick;
Blip g_hcsBlip;
size_t g_numCollected = 0;
size_t g_numTotal = 0;
bool g_hasCollectedAll = false;
bool g_isAtComicStore = false;
fs::path g_modDir = fs::current_path().append("mod_" + Constants::ModName);
std::vector<Figure> g_figures;
std::string g_storeInfoText;

// Script config
bool g_figureBlips = false;
bool g_comicStoreMessages = true;
int g_figureReward = 250;

void CreateRewardPickups()
{
	int flags = 0;
	MISC::SET_BIT(&flags, 2);
	MISC::SET_BIT(&flags, 1);
	MISC::SET_BIT(&flags, 4);
	MISC::SET_BIT(&flags, 3);

	OBJECT::CREATE_PICKUP_ROTATE(
		MISC::GET_HASH_KEY("PICKUP_WEAPON_RAYPISTOL"),
		-143.640f, 231.6f, 94.65f,
		180.0f, 0.0f, 180.0f,
		flags, 9999, 2, true, 0
	);

	OBJECT::CREATE_PICKUP_ROTATE(
		MISC::GET_HASH_KEY("PICKUP_WEAPON_RAYCARBINE"),
		-143.780f, 230.6f, 94.65f,
		180.0f, 0.0f, 180.0f,
		flags, 9999, 2, true, 0
	);

	OBJECT::CREATE_PICKUP_ROTATE(
		MISC::GET_HASH_KEY("PICKUP_WEAPON_RAYMINIGUN"),
		-143.880f, 229.6f, 94.65f,
		180.0f, 0.0f, 180.0f,
		flags, 9999, 2, true, 0
	);
}

void SaveFigures()
{
	pugi::xml_document doc;
	pugi::xml_node root = doc.append_child("figures");

	for (Figure& figure : g_figures)
	{
		pugi::xml_node figureNode = root.append_child("figure");
		figureNode.append_attribute("type") = figure.GetTypeString().c_str();
		figureNode.append_attribute("posX") = figure.GetX();
		figureNode.append_attribute("posY") = figure.GetY();
		figureNode.append_attribute("posZ") = figure.GetZ();
		figureNode.append_attribute("rotX") = figure.GetRotX();
		figureNode.append_attribute("rotY") = figure.GetRotY();
		figureNode.append_attribute("rotZ") = figure.GetRotZ();
		figureNode.append_attribute("collected") = figure.IsCollected();
	}

	fs::path figureDataPath = g_modDir;
	figureDataPath.append("figures.xml");

	doc.save_file(figureDataPath.c_str(), "\t", pugi::format_default | pugi::format_no_declaration);
}

void ScriptInit()
{
	ScriptLog.Write(LogLevel::LOG_DEBUG, "ScriptInit called");

	// Load figure data
	fs::path figureDataPath = g_modDir;
	figureDataPath.append("figures.xml");
	ScriptLog.Write(LogLevel::LOG_DEBUG, fmt::format("Figures path: {}", figureDataPath.generic_string()));

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(figureDataPath.c_str());
	if (result)
	{
		pugi::xml_node root = doc.child("figures");

		for (pugi::xml_node node : root.children("figure"))
		{
			bool collected = node.attribute("collected").as_bool();

			g_figures.emplace_back(
				node.attribute("type").as_string(),
				node.attribute("posX").as_float(), node.attribute("posY").as_float(), node.attribute("posZ").as_float(),
				node.attribute("rotX").as_float(), node.attribute("rotY").as_float(), node.attribute("rotZ").as_float(),
				collected
			);

			if (collected) g_numCollected++;
		}

		g_numTotal = g_figures.size();
		g_hasCollectedAll = g_numCollected >= g_numTotal;
		g_storeInfoText = fmt::format("You've collected {} out of {} action figures.", g_numCollected, g_numTotal);

		ScriptLog.Write(LogLevel::LOG_INFO, fmt::format("Loaded {} figures.", g_numTotal));
	}
	else
	{
		ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("Failed to load figure data. ({})", result.description()));
	}

	// Load user config
	fs::path configPath = g_modDir;
	configPath.append("config.ini");
	ScriptLog.Write(LogLevel::LOG_DEBUG, fmt::format("Config path: {}", configPath.generic_string()));

	CSimpleIniA config;
	config.SetUnicode();

	SI_Error configResult = config.LoadFile(configPath.c_str());
	if (configResult == SI_Error::SI_OK)
	{
		g_figureBlips = config.GetBoolValue("CONFIG", "bFiguresAlwaysVisible", false);
		g_comicStoreMessages = config.GetBoolValue("CONFIG", "bStoreMessages", true);
		g_figureReward = config.GetLongValue("CONFIG", "iFigureReward", 250);

		ScriptLog.Write(LogLevel::LOG_INFO, "Loaded config");
		ScriptLog.Write(LogLevel::LOG_DEBUG, fmt::format("g_figureBlips: {}", g_figureBlips));
		ScriptLog.Write(LogLevel::LOG_DEBUG, fmt::format("g_comicStoreMessages: {}", g_comicStoreMessages));
		ScriptLog.Write(LogLevel::LOG_DEBUG, fmt::format("g_figureReward: {}", g_figureReward));
	}
	else
	{
		ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("Failed to load config, using default values. (Code {})", configResult));
	}

	// Load audio bank
	AUDIO::REQUEST_SCRIPT_AUDIO_BANK("DLC_VINEWOOD/DLC_VW_HIDDEN_COLLECTIBLES", false, -1);

	// Create store blip
	g_hcsBlip = HUD::ADD_BLIP_FOR_COORD(Constants::ComicStoreX, Constants::ComicStoreY, Constants::ComicStoreZ);
	HUD::SET_BLIP_SPRITE(g_hcsBlip, 671);
	HUD::SET_BLIP_PRIORITY(g_hcsBlip, 11);
	HUD::SET_BLIP_AS_SHORT_RANGE(g_hcsBlip, true);

	// Figure map icons
	if (g_figureBlips)
	{
		for (Figure& figure : g_figures) figure.CreateBlip();
	}

	// Store message
	if (g_comicStoreMessages)
	{
		Util::NotifyWithPicture("CELL_COMIC_N", "CHAR_COMIC_STORE", g_hasCollectedAll ? "ACFIG_TEXT2" : "ACFIG_TEXT1");
	}

	// Update loop & rewards
	if (g_hasCollectedAll)
	{
		CreateRewardPickups();
		ScriptLog.Write(LogLevel::LOG_DEBUG, "Won't start update loop because all figures are collected");
		ScriptLog.Write(LogLevel::LOG_INFO, "You've collected all figures, congrats!");
	}
	else
	{
		ScriptLog.Write(LogLevel::LOG_DEBUG, "Starting update loop");
		ScriptUpdate();
	}
}

void ScriptUpdate()
{
	while (!g_hasCollectedAll)
	{
		g_currentTick = GetTickCount();

		if (g_isAtComicStore)
		{
			Util::DisplayHelpTextThisFrame(g_storeInfoText.c_str());
		}

		if (g_currentTick >= g_nextUpdateTick)
		{
			Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);

			g_nextUpdateTick = g_currentTick + Constants::Heartbeat;
			g_isAtComicStore = MISC::GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, Constants::ComicStoreX, Constants::ComicStoreY, Constants::ComicStoreZ, true) <= Constants::StoreRange;

			for (Figure& figure : g_figures)
			{
				if (MISC::GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, figure.GetX(), figure.GetY(), figure.GetZ(), true) <= Constants::PickupRange)
				{
					figure.CreatePickup();

					if (!figure.IsCollected() && !figure.HasPickupObject())
					{
						AUDIO::PLAY_SOUND_FRONTEND(-1, Figure::GetFigureTypeAudio(figure.GetType()).c_str(), "dlc_vw_hidden_collectible_sounds", false);

						figure.SetCollected(true);
						figure.DestroyPickup();
						figure.DestroyBlip();

						g_numCollected++;
						g_storeInfoText = fmt::format("You've collected {} out of {} action figures.", g_numCollected, g_numTotal);

						Util::Notify(fmt::format("Action Figures: {}/{}", g_numCollected, g_numTotal));
						Util::GiveMoney(g_figureReward);
						SaveFigures();

						if (g_numCollected >= g_numTotal)
						{
							g_hasCollectedAll = true;

							CreateRewardPickups();
							Util::NotifyWithPicture("CELL_COMIC_N", "CHAR_COMIC_STORE", "ACFIG_TEXT2");
							Util::DisplayHelpTextTimed("HELPACFIGCOLALL", Constants::InfoTime);

							HUD::SET_BLIP_FLASHES(g_hcsBlip, true);
							HUD::SET_BLIP_FLASH_TIMER(g_hcsBlip, Constants::InfoTime);
						}
					}
				}
				else
				{
					figure.DestroyPickup();
				}
			}
		}

		WAIT(0);
	}

	ScriptLog.Write(LogLevel::LOG_DEBUG, "Update loop stopped");
}