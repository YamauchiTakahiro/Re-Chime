#include "stdafx.h"
#include "DifficultyLevel.h"
#include "Game.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Actor/Character/Enemy/SmallRobot/SmallRobot.h"
#include "Source/Camera/GameCamera.h"
#include "Source/UIBase/UI/UI.h"
//#include "Source/Sound/BGM/BGMManager.h"

DifficultyLevel::DifficultyLevel()
{

}

DifficultyLevel::~DifficultyLevel()
{

}

bool DifficultyLevel::Start()
{
	m_Title.Init("Assets/Sprite/Title.DDs", 1920.0f, 1080.0f);
	m_DifficultyLevel.Init("Assets/UIData/HP.DDs", 1024.0f, 128.0f);
	m_DifficultyLevel.SetPosition(Vector3(960.0f, 540.0f, 0.0f));
	m_DifficultyLevel.SetScale(Vector3(0.41f, 3.0f, 0.5f));
	m_DifficultyLevel.SetPivot(Vector2(0.0f, 0.5f));
	return false;
}

void DifficultyLevel::Update()
{

}

void DifficultyLevel::Render(RenderContext& rc)
{
	m_Title.Draw(rc);
}
