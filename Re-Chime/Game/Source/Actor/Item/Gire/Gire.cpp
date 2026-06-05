#include "stdafx.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Manager/AudioManager/AudioManager.h"
#include "collision/CollisionObject.h"

Gire::Gire()
{

}

Gire::~Gire()
{

}

bool Gire::Start()
{
	m_modelRender.Init("Assets/modelData/Item/Gear/Gear.tkm");
	m_player = FindGO<Player>("player");
	m_audioManager = FindGO<AudioManager>("audioManager");

	return true;
}

void Gire::Update()
{
	Move();

	bool isGetGire = m_player->GetGier();
	if (isGetGire == true)
	{
		GetGier();
	}
	m_modelRender.Update();
	m_modelRender.SetScale(m_scale);
}

void Gire::Move()
{
	m_modelRender.SetPosition(m_position);
	if (m_collisionObject != nullptr)
	{
		m_collisionObject->SetPosition(m_position);
	}
}

void Gire::GetGier()
{
	m_player->addGireCount();
	m_audioManager->PlaySE(enSound_GetGearSE, 1.0f, enSEPlay_AllowOverlap);
	DeleteGO(this);
}

void Gire::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}