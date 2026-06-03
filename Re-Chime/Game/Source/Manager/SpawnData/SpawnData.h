#pragma once
#include "stdafx.h"

enum class enSpawnType
{
	enSpawnType_SmallRobot,
	enSpawnType_MediumRobot,
	enSpawnType_FloorBoss,
	enSpawnType_FinalBoss,
	enSpawnType_Barrier,
};

struct SpawnData
{
	enSpawnType spawnType;
	Vector3 pos;
	Quaternion rot;
	Vector3 scale;
	int floorNum;
	bool spawned = false;
};