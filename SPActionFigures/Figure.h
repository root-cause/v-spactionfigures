#pragma once
#include <string>
#include "ScriptHookV/inc/types.h"
#include "ScriptHookV/inc/natives.h"

enum FigureType
{
	FT_INVALID,
	FT_SPACE_MONKEY,
	FT_ALIEN,
	FT_IMPOTENT_RAGE,
	FT_PRINCESS_BUBBLEGUM,
	FT_SPACE_RANGER_COMMANDER,
	FT_SPACE_RANGER,
	FT_BEAST,
	FT_SASQUATCH
};

class Figure
{
private:
	std::string m_typeStr;
	FigureType m_type;
	float m_posX;
	float m_posY;
	float m_posZ;
	float m_rotX;
	float m_rotY;
	float m_rotZ;
	bool m_collected;
	Blip m_blip;
	Pickup m_pickup;
	DWORD m_pickupReadyTick;

public:
	static FigureType GetFigureType(const std::string& figureTypeStr);
	static Hash GetFigureTypeModel(FigureType type);
	static std::string GetFigureTypeAudio(FigureType type);

	Figure(const std::string& typeName, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, bool collected);
	std::string GetTypeString() const;
	FigureType GetType() const;
	float GetX() const;
	float GetY() const;
	float GetZ() const;
	float GetRotX() const;
	float GetRotY() const;
	float GetRotZ() const;
	void CreateBlip();
	void DestroyBlip();
	void CreatePickup();
	void DestroyPickup();
	bool HasPickupObject() const;
	bool IsCollected() const;
	void SetCollected(bool collected);
};

extern DWORD g_currentTick;