#include "stdafx.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "collision/CollisionObject.h"

Gire::Gire()
{

}

Gire::~Gire()
{
	DeleteGO(m_collisionObject);
}

bool Gire::Start()
{
	m_modelRender.Init("Assets/modelData/Item/Gear/Gear.tkm");
	m_player = FindGO<Player>("player");
	m_audioManager = FindGO<AudioManager>("audioManager");
	SetScale();

	SetScale();

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

void Gire::SetScale()
{
	m_scale.Set(3.0f, 3.0f, 3.0f);
}

void Gire::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}