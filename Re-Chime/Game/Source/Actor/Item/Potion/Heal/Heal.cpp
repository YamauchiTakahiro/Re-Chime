#include "stdafx.h"
#include "Heal.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Character/Player/Player.h"

Heal::Heal()
{
}

Heal::~Heal()
{
	DeleteGO(m_collisionObject);
}

bool Heal::Start()
{
	m_modelRender.Init("Assets/modelData/Item/Potion/Heal.tkm");
	m_player = FindGO<Player>("player");
	Collision();
	return true;
}

void Heal::Update()
{
	Rotation();
	Move();
	Delete();
	m_modelRender.Update();
}

void Heal::Move()
{
	m_modelRender.SetPosition(m_position);
}

void Heal::Rotation()
{
}

void Heal::Collision()
{
	m_collisionObject = NewGO<CollisionObject>(0);
	Vector3 collisionPos = m_position;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("healPotion");
	m_collisionObject->SetIsEnableAutoDelete(false);
}

void Heal::Delete()
{
	bool isHit = false;
	isHit = m_player->GetHealFlag(isHit);
	if (isHit)
	{
		isHit = false;
		DeleteGO(this);
	}
}

void Heal::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}

