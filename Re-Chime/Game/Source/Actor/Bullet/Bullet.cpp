#include "stdafx.h"
#include "Bullet.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Character/Player/Player.h"

Bullet::Bullet()
{
	
}

Bullet::~Bullet()
{
	if (m_collisionObject != nullptr)
	{
		DeleteGO(m_collisionObject);
		m_collisionObject = nullptr;
	}
}

bool Bullet::Start()
{
	m_modelRender.Init("Assets/modelData/Item/Bullet/bullet.tkm");

	OnCollision();

	SetScale();

	m_player = FindGO<Player>("player");

	return true;
}

void Bullet::Update()
{
	Move();

	Rotation();

	Hit();

	// コリジョン位置更新
	if (m_collisionObject != nullptr)
	{
		m_collisionObject->SetPosition(m_position);
	}

	// 一定距離で削除
	if ((m_position - m_startPosition).Length() >= 3000.0f)
	{
		DeleteGO(this);
	}

	m_modelRender.Update();
}

void Bullet::Move()
{
	m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime();

	m_modelRender.SetPosition(m_position);
}

void Bullet::Rotation()
{
	Vector3 forward = m_moveSpeed;

	forward.y = 0.0f;

	if (forward.LengthSq() <= 0.0001f)
	{
		return;
	}

	forward.Normalize();

	Quaternion rot;
	rot.SetRotationYFromDirectionXZ(forward);

	// モデルの軸補正
	Quaternion fixRot;
	fixRot.SetRotationDegX(90.0f);

	rot *= fixRot;

	m_modelRender.SetRotation(rot);
}

void Bullet::SetScale()
{
	m_scale = Vector3(5.0f, 5.0f, 5.0f);
	m_modelRender.SetScale(m_scale);
}

void Bullet::Render(RenderContext& renderConetxt)
{
	m_modelRender.Draw(renderConetxt);
}

void Bullet::OnCollision()
{
	m_collisionObject = NewGO<CollisionObject>(0);
	Vector3 collisionPosition = m_position;

	m_collisionObject->CreateSphere(collisionPosition, Quaternion::Identity, 20.0f);
	m_collisionObject->SetName("bullet");
	m_collisionObject->SetIsEnableAutoDelete(false);
}

void Bullet::Hit()
{
	const auto& collisions =
		g_collisionObjectManager->FindCollisionObjects("bullet");

	for (auto collision : collisions)
	{
		if (collision->IsHit(m_player->GetCharacterController()))
		{
			m_player->TakeDamage(m_attackPower, m_position);
			DeleteGO(this);
			break;
		}
	}
}