#include "Script.h"
#include "Figure.h"
#include "Util.h"
#include "Logger.h"

FigureType Figure::GetFigureType(const std::string& figureTypeStr)
{
	if (figureTypeStr == "pogo") {
		return FigureType::FT_SPACE_MONKEY;
	}
	else if (figureTypeStr == "alien") {
		return FigureType::FT_ALIEN;
	}
	else if (figureTypeStr == "imporage") {
		return FigureType::FT_IMPOTENT_RAGE;
	}
	else if (figureTypeStr == "prbubble") {
		return FigureType::FT_PRINCESS_BUBBLEGUM;
	}
	else if (figureTypeStr == "rsrcomm") {
		return FigureType::FT_SPACE_RANGER_COMMANDER;
	}
	else if (figureTypeStr == "rsrgeneric") {
		return FigureType::FT_SPACE_RANGER;
	}
	else if (figureTypeStr == "beast") {
		return FigureType::FT_BEAST;
	}
	else if (figureTypeStr == "sasquatch") {
		return FigureType::FT_SASQUATCH;
	}
	else {
		ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("GetFigureType :: Invalid figureTypeStr: {}", figureTypeStr));
		return FigureType::FT_INVALID;
	}
}

Hash Figure::GetFigureTypeModel(FigureType type)
{
	switch (type)
	{
		case FigureType::FT_SPACE_MONKEY:
			return MISC::GET_HASH_KEY("vw_prop_vw_colle_pogo");

		case FigureType::FT_ALIEN:
			return MISC::GET_HASH_KEY("vw_prop_vw_colle_alien");

		case FigureType::FT_IMPOTENT_RAGE:
			return MISC::GET_HASH_KEY("vw_prop_vw_colle_imporage");

		case FigureType::FT_PRINCESS_BUBBLEGUM:
			return MISC::GET_HASH_KEY("vw_prop_vw_colle_prbubble");

		case FigureType::FT_SPACE_RANGER_COMMANDER:
			return MISC::GET_HASH_KEY("vw_prop_vw_colle_rsrcomm");

		case FigureType::FT_SPACE_RANGER:
			return MISC::GET_HASH_KEY("vw_prop_vw_colle_rsrgeneric");

		case FigureType::FT_BEAST:
			return MISC::GET_HASH_KEY("vw_prop_vw_colle_beast");

		case FigureType::FT_SASQUATCH:
			return MISC::GET_HASH_KEY("vw_prop_vw_colle_sasquatch");

		default:
			ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("GetFigureTypeModel :: Invalid type: {}", type));
			return MISC::GET_HASH_KEY("vw_prop_vw_colle_sasquatch");
	}
}

std::string Figure::GetFigureTypeAudio(FigureType type)
{
	switch (type)
	{
		case FigureType::FT_SPACE_MONKEY:
			return "pogo_space_monkey";
		
		case FigureType::FT_ALIEN:
			return "alien";

		case FigureType::FT_IMPOTENT_RAGE:
			return "impotent_rage";

		case FigureType::FT_PRINCESS_BUBBLEGUM:
			return "princess_robot_bubblegum";

		case FigureType::FT_SPACE_RANGER_COMMANDER:
			return "space_ranger_commander";

		case FigureType::FT_SPACE_RANGER:
			return "republican_space_ranger";

		case FigureType::FT_BEAST:
			return "beast";

		case FigureType::FT_SASQUATCH:
			return "sasquatch";
		
		default:
			ScriptLog.Write(LogLevel::LOG_ERROR, fmt::format("GetFigureTypeAudio :: Invalid figureType: {}", type));
			return "sasquatch";
	}
}

Figure::Figure(const std::string& typeName, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, bool collected)
{
	m_typeStr = typeName;
	m_type = GetFigureType(typeName);
	m_posX = posX;
	m_posY = posY;
	m_posZ = posZ;
	m_rotX = rotX;
	m_rotY = rotY;
	m_rotZ = rotZ;
	m_collected = collected;
	m_blip = 0;
	m_pickup = 0;
	m_pickupReadyTick = 0;
}

std::string Figure::GetTypeString() const
{
	return m_typeStr;
}

FigureType Figure::GetType() const
{
	return m_type;
}

float Figure::GetX() const
{
	return m_posX;
}

float Figure::GetY() const
{
	return m_posY;
}

float Figure::GetZ() const
{
	return m_posZ;
}

float Figure::GetRotX() const
{
	return m_rotX;
}

float Figure::GetRotY() const
{
	return m_rotY;
}

float Figure::GetRotZ() const
{
	return m_rotZ;
}

void Figure::CreateBlip()
{
	if (m_collected || m_blip != 0)
	{
		return;
	}

	m_blip = HUD::ADD_BLIP_FOR_COORD(m_posX, m_posY, m_posZ);
	HUD::SET_BLIP_SPRITE(m_blip, 671);
	HUD::SET_BLIP_COLOUR(m_blip, 3);
	HUD::SET_BLIP_SCALE(m_blip, 0.8f);
	HUD::SET_BLIP_AS_SHORT_RANGE(m_blip, true);

	HUD::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Action Figure");
	HUD::END_TEXT_COMMAND_SET_BLIP_NAME(m_blip);
}

void Figure::DestroyBlip()
{
	if (m_blip == 0)
	{
		return;
	}

	// No need to set m_blip to 0 because the native does it
	HUD::REMOVE_BLIP(&m_blip);
}

void Figure::CreatePickup()
{
	if (m_collected || m_pickup != 0)
	{
		return;
	}

	// Load pickup model
	Hash model = GetFigureTypeModel(m_type);
	if (!STREAMING::IS_MODEL_VALID(model))
	{
		return;
	}

	Util::LoadModel(model);

	// Create the pickup
	int flags = 0;
	MISC::SET_BIT(&flags, 5);
	MISC::SET_BIT(&flags, 8);
	MISC::SET_BIT(&flags, 1);
	MISC::SET_BIT(&flags, 0);
	MISC::SET_BIT(&flags, 16);

	m_pickup = OBJECT::CREATE_PICKUP_ROTATE(
		MISC::GET_HASH_KEY("PICKUP_PORTABLE_CRATE_UNFIXED_INCAR_WITH_PASSENGERS"),
		m_posX, m_posY, m_posZ,
		m_rotX, m_rotY, m_rotZ,
		flags,
		0,
		2,
		true,
		model
	);

	m_pickupReadyTick = g_currentTick + 1000;

	// Release pickup model
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
}

void Figure::DestroyPickup()
{
	if (m_pickup == 0)
	{
		return;
	}

	OBJECT::REMOVE_PICKUP(m_pickup);

	m_pickup = 0;
	m_pickupReadyTick = 0;
}

bool Figure::HasPickupObject() const
{
	// Teleporting + DOES_PICKUP_OBJECT_EXIST sometimes causes figures to get collected instantly, so I added a 1 sec delay
	if (g_currentTick < m_pickupReadyTick)
	{
		return true;
	}

	return OBJECT::DOES_PICKUP_OBJECT_EXIST(m_pickup);
}

bool Figure::IsCollected() const
{
	return m_collected;
}

void Figure::SetCollected(bool collected)
{
	m_collected = collected;
}