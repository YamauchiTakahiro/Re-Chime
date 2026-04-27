#include "stdafx.h"
#include "PowerBuff.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Character/Player/Player.h"

PowerBuff::PowerBuff()
{
}

PowerBuff::~PowerBuff()
{
	DeleteGO(m_collisionObject);
}

bool PowerBuff::Start()
{
	m_modelRender.Init("Assets/modelData/Item/Potion/PowerBuffPotion.tkm");
	m_player = FindGO<Player>("player");
	Collision();
	return true;
}

void PowerBuff::Update()
{
	Move();
	Rotation();
	Delete();
	m_modelRender.Update();
}

void PowerBuff::Move()
{
	m_modelRender.SetPosition(m_position);
}

void PowerBuff::Rotation()
{
}

void PowerBuff::Collision()
{
	m_collisionObject = NewGO<CollisionObject>(0);
	Vector3 collisionPos = m_position;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("powerBuffPotion");
	m_collisionObject->SetIsEnableAutoDelete(false);
}

void PowerBuff::Delete()
{
	bool isHit = m_player->GetPowerBuffFlag();
	if (isHit == true)
	{
		DeleteGO(this);
	}
}

void PowerBuff::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}