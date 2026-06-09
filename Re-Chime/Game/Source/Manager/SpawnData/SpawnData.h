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
	enSpawnType spawnType = enSpawnType::enSpawnType_SmallRobot;
	Vector3 pos = Vector3::Zero;
	Quaternion rot = Quaternion::Identity;
	Vector3 scale = Vector3::Zero;
	int floorNum = 0;
	bool spawned = false;
};