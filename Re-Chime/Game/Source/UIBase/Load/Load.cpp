#include "stdafx.h"
#include "Load.h"
#include "Game.h"

Load::Load()
{
	m_Load.Init("Assets/Sprite/Load.DDs", 1920.0f, 1080.0f);
	m_gear.Init("Assets/UIData/gear.DDs", 128.0f, 128.0f);
	m_gear.SetPosition(Vector3(800.0f, -425.0f, 0.0f));
	m_gear.SetScale(Vector3(1.0f, 1.0f, 1.0f));
	m_gear.Update();

	m_BarFrame.Init("Assets/UIData/HP.DDs", 400.0f, 40.0f);
	m_BarFrame.SetPosition(Vector3(-955.0f, -525.0f, 0.0f));
	m_BarFrame.SetPivot(Vector2(0.0f, 0.5f));
	m_BarFrame.Update();
	
	LoadTime = 350;
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


	//ローディングバーの拡大
	Vector3 scale = m_BarFrame.GetScale();
	scale.x = m_BarFrame.GetScale().x + 0.7f * g_gameTime->GetFrameDeltaTime();
	m_BarFrame.SetScale(scale);
	m_BarFrame.Update();
}

void Load::Render(RenderContext& rc)
{
	m_Load.Draw(rc);
	m_gear.Draw(rc);
	m_BarFrame.Draw(rc);
}