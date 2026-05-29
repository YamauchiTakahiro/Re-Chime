#include "stdafx.h"
#include "Heal.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Manager/AudioManager/AudioManager.h"

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
	m_audioManager = FindGO<AudioManager>("audioManager");
	m_oldPotionCount = m_player->GetHealPotionCount();
	Collision();
	return true;
}

void Heal::Update()
{
	Rotation();
	Move();
	SetScale();
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

void Heal::SetScale()
{
	m_modelRender.SetScale(Vector3(2.0f, 2.0f, 2.0f));
}

void Heal::Delete()
{
	if (m_player->GetHealPotionCount() > m_oldPotionCount)
	{
		m_audioManager->PlaySE(
			enSound_GetItemSE,
			1.0f,
			enSEPlay_AllowOverlap
		);

		DeleteGO(this);
	}
}

void Heal::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}

