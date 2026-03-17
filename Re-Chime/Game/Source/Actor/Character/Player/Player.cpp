#include "stdafx.h"
#include "Player.h"

Player::Player()
{
	//モデルを初期化する。
	m_modelRender.Init("Assets/modelData/unityChan.tkm");
	m_characterController.Init(25.0f, 75.0f, m_position);
}

Player::~Player()
{

}

bool Player::Start()
{
	return true;
}

void Player::Update()
{
	Move();

	Rotation();

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
		right *= stickL.x * 1000.0f;
		forward *= stickL.y * 1000.0f;
	}

	m_moveSpeed += right + forward;

	//二段ジャンプ
	if (m_characterController.IsOnGround())
	{
		m_moveSpeed.y = 0.0f;
		m_doubleJump = false;
		if (g_pad[0]->IsTrigger(enButtonA))
		{
			m_moveSpeed.y = 240.0f;
		}
		if (m_characterController.IsOnGround() == false && m_doubleJump == false)
		{
			if (g_pad[0]->IsTrigger(enButtonA))
			{
				m_moveSpeed.y = 240.0f;
				m_doubleJump = true;
			}
		}
	}
	if (m_characterController.IsOnGround() == false)
	{
		//m_moveSpeed.y -= 8.0f;
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

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}