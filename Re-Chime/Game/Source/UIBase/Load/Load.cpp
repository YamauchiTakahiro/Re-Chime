#include "stdafx.h"
#include "Load.h"
#include "Game.h"

Load::Load()
{
	m_Load.Init("Assets/Sprite/Load.DDs", 1920.0f, 1080.0f);
	m_gear.Init("Assets/UIData/gear.DDs", 128.0f, 128.0f);
	m_gear.SetPosition(Vector3(-600.0f, -350.0f, 0.0f));
	m_gear.SetScale(Vector3(1.0f, 1.0f, 1.0f));
	m_gear.Update();
	LoadTime = 10;
	m_GearRotSpeed = 5.0f;
}

Load::~Load()
{

}

bool Load::Start()
{
	return true;
}

void Load::Update()
{
	if (LoadTime > 0)
	{
		LoadTime--;
	}
	else
	{
		NewGO<Game>(0, "game");
		DeleteGO(this);
	}
	m_time += g_gameTime->GetFrameDeltaTime();
	//ギアの回転
	m_GearRotation.SetRotationZ(m_time * m_GearRotSpeed);
	m_gear.SetRotation(m_GearRotation);
	m_gear.Update();
}

void Load::Render(RenderContext& rc)
{
	m_Load.Draw(rc);
	m_gear.Draw(rc);
}