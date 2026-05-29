#include "stdafx.h"
#include "AttackSpeedBuff.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Manager/AudioManager/AudioManager.h"

AttackSpeedBuff::AttackSpeedBuff()
{
}

AttackSpeedBuff::~AttackSpeedBuff()
{
	DeleteGO(m_collisionObject);
}

bool AttackSpeedBuff::Start()
{
	m_modelRender.Init("Assets/modelData/Item/Potion/SpeedBuffPotion.tkm");
	m_player = FindGO<Player>("player");
	m_audioManager = FindGO<AudioManager>("audioManager");
	m_oldPotionCount = m_player->GetAttackSpeedPotionCount();
	Collision();
	return true;
}

void AttackSpeedBuff::Update()
{
	Move();
	Rotation();
	SetScale();
	Delete();
	m_modelRender.Update();
}

void AttackSpeedBuff::Move()
{
	m_modelRender.SetPosition(m_position);
}

void AttackSpeedBuff::Rotation()
{
}

void AttackSpeedBuff::Collision()
{
	m_collisionObject = NewGO<CollisionObject>(0);

	Vector3 collisionPos = m_position;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("attackSpeedBuffPotion");
	m_collisionObject->SetIsEnableAutoDelete(false);
}

void AttackSpeedBuff::SetScale()
{
	m_modelRender.SetScale(Vector3(2.0f, 2.0f, 2.0f));
}

void AttackSpeedBuff::Delete()
{
	if (m_player->GetAttackSpeedPotionCount() > m_oldPotionCount)
	{
		m_audioManager->PlaySE(
			enSound_GetItemSE,
			1.0f,
			enSEPlay_AllowOverlap
		);

		DeleteGO(this);
	}
}

void AttackSpeedBuff::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}