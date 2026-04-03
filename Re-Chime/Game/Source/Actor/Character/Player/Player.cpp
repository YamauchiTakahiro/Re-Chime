#include "stdafx.h"
#include "Player.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Item/Gire/Gire.h"

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Start()
{
	//モデルを初期化する。
	m_modelRender.Init("Assets/modelData/Player/Player.tkm");
	m_characterController.Init(100.0f, 300.0f, m_position);
	m_gire = FindGO<Gire>("gire");
	return true;
}

void Player::Update()
{
	Move();

	Rotation();

	Attack();

	Time();

	Hit();

	DamageIntarval();
	m_modelRender.Update();
}

void Player::Move()
{
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;

	Vector3 stickL;
	stickL.x = g_pad[0]->GetLStickXF();
	stickL.y = g_pad[0]->GetLStickYF();

	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();

	forward.y = 0.0f;
	right.y = 0.0f;

	if (g_pad[0]->IsPress(enButtonY) == false)
	{
		right *= stickL.x * 120.0f;
		forward *= stickL.y * 120.0f;
	}

	if (g_pad[0]->IsPress(enButtonY))
	{
		right *= stickL.x * 240.0f;
		forward *= stickL.y * 240.0f;
	}

	m_moveSpeed += right + forward;

	//二段ジャンプ
	if (m_characterController.IsOnGround())
	{
		m_moveSpeed.y = 0.0f;
		if (g_pad[0]->IsTrigger(enButtonB))
		{
			m_moveSpeed.y = 500.0f;
		}
	}
	if (m_characterController.IsOnGround() == false)
	{
		m_moveSpeed.y -= 20.0f;
	}

	m_position = m_characterController.Execute(m_moveSpeed, 2.0f / 60.0f);

	m_modelRender.SetPosition(m_position);
}

void Player::Rotation()
{
	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		m_rotation.SetRotationYFromDirectionXZ(m_moveSpeed);

		m_modelRender.SetRotation(m_rotation);
	}
}

void Player::Attack()
{
	if (g_pad[0]->IsTrigger(enButtonA) && m_timeCount == 0.0f)
	{
		OnCollision();
		m_timeCount = 1.0f;
		Time();
	}
}

void Player::OnCollision()
{
	m_collisionObject = NewGO<CollisionObject>(0);

	Vector3 collisionPos = m_position;
	m_forward = Vector3::Front;
	m_rotation.Apply(m_forward);
	collisionPos += m_forward * 250.0f;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("playerAttack");
}

void Player::Time()
{
	m_timeCount -= g_gameTime->GetFrameDeltaTime();
	if (m_timeCount < 0.0f)
	{
		m_timeCount = 0.0f;
	}

}

void Player::Hit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("smallRobotAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_characterController) == true && m_damageIntarvalTime == 0.0f)
		{
			m_playerHp -= 10;
			m_damageIntarvalTime = 1.0f;
		}
	}

	const auto& collisions2 = g_collisionObjectManager->FindCollisionObjects("floorBossAttack");
	for (auto collision : collisions2)
	{
		if (collision->IsHit(m_characterController) == true && m_damageIntarvalTime == 0.0f)
		{
			m_playerHp -= 20;
			m_damageIntarvalTime = 1.0f;
		}
	}
}

void Player::DamageIntarval()
{
	m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_damageIntarvalTime < 0.0f)
	{
		m_damageIntarvalTime = 0.0f;
	}
}

void Player::GetGier()
{
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}