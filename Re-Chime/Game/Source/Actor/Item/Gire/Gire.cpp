#include "stdafx.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Source/Actor/Character/Player/Player.h"
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
	OnCollision();
	return true;
}

void Gire::Update()
{
	Move();

	bool isGetGire = false;
	m_player->GetGier(false);
	if (m_player->GetGier(isGetGire) == true)
	{
		GetGier();
	}
	m_modelRender.Update();
}

void Gire::Move()
{
	m_modelRender.SetPosition(m_position);
}

void Gire::OnCollision()
{
	m_collisionObject = NewGO<CollisionObject>(0);
	Vector3 collisionPos = m_position;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 100.0f);
	m_collisionObject->SetName("gireCollision");
	m_collisionObject->SetIsEnableAutoDelete(false);
}

void Gire::GetGier()
{
	m_player->GetGireCount();
	DeleteGO(this);
}

void Gire::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}