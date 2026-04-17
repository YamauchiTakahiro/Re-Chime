#include "stdafx.h"
#include "Player.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Game.h"
#include "Source/Actor/Character/Enemy/SmallRobot/SmallRobot.h"
#include "Source/Actor/Character/Enemy/MediumRobot/MediumRobot.h"
#include "Source/Actor/Character/Enemy/FloorBoss/FloorBoss.h"
#include "Source/Actor/Character/Enemy/FinalBoss/FinalBoss.h"

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Player/playerIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Player/playerWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Jump].Load("Assets/animData/Player/playerJump.tka");
	m_animationClips[enAnimationClip_Jump].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/Player/playerRun.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	//モデルを初期化する。
	m_modelRender.Init("Assets/modelData/Player/player.tkm", m_animationClips, enAnimationClip_Num);
	m_characterController.Init(100.0f, 300.0f, m_position);
	m_gire = FindGO<Gire>("gire");
	m_game = FindGO<Game>("game");	
	m_smallRobot = FindGO<SmallRobot>("smallRobot");
	m_mediumRobot = FindGO<MediumRobot>("mediumRobot");
	m_floorBoss = FindGO<FloorBoss>("FloorBoss");
	m_finalBoss = FindGO<FinalBoss>("finalBoss");
	return true;
}

void Player::Update()
{
	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);
	if (isPause)
	{
		return;
	}
	Move();

	Rotation();

	SetScale();

	Attack();

	Time();

	Hit();

	DamageIntarval();

	Gurad();

	PowerBuff();

	PowerBuffTime();

	AttackSpeedBuffTime();

	PlayerState();

	PlayAnimation();
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
		right *= stickL.x * 240.0f;
		forward *= stickL.y * 240.0f;
	}

	if (g_pad[0]->IsPress(enButtonY))
	{
		right *= stickL.x * 480.0f;
		forward *= stickL.y * 480.0f;
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

void Player::SetScale()
{
	m_scale.Set(7.0f, 7.0f, 7.0f);
	m_modelRender.SetScale(m_scale);
}

void Player::Attack()
{
	if (g_pad[0]->IsTrigger(enButtonA) && m_timeCount == 0.0f)
	{
		OnCollision();
		if(m_attackSpeedBuffFlag == true)
		{
			m_timeCount = 1.0f; // 攻撃クールタイムを短縮する例
		}
		else if (m_attackSpeedBuffFlag == false)
		{
			m_timeCount = 2.0f; // 通常の攻撃クールタイム
		}
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
			if (m_guardFlag == false)
			{
				int smallRobotAttackPower = 0;
				smallRobotAttackPower = m_smallRobot->GetAttackPower(smallRobotAttackPower);
				m_playerHp -= smallRobotAttackPower;
			}
			else if (m_guardFlag == true)
			{
				int smallRobotAttackPower = 0;
				smallRobotAttackPower = m_smallRobot->GetAttackPower(smallRobotAttackPower);
				m_playerHp -= smallRobotAttackPower / 2;
			}
			m_damageIntarvalTime = 1.0f;
		}
	}

	const auto& collisions2 = g_collisionObjectManager->FindCollisionObjects("mediumRobotAttack");
	for (auto collision : collisions2)
	{
		if (collision->IsHit(m_characterController) == true && m_damageIntarvalTime == 0.0f)
		{
			if (m_guardFlag == false)
			{
				int mediumRobotAttackPower = 0;
				mediumRobotAttackPower = m_mediumRobot->GetAttackPower(mediumRobotAttackPower);
				m_playerHp -= mediumRobotAttackPower;
			}
			else if (m_guardFlag == true)
			{
				int mediumRobotAttackPower = 0;
				mediumRobotAttackPower = m_mediumRobot->GetAttackPower(mediumRobotAttackPower);
				m_playerHp -= mediumRobotAttackPower / 2;
			}
			m_damageIntarvalTime = 1.0f;
		}
	}

	const auto& collisions3 = g_collisionObjectManager->FindCollisionObjects("floorBossAttack");
	for (auto collision : collisions3)
	{
		if (collision->IsHit(m_characterController) == true && m_damageIntarvalTime == 0.0f)
		{
			if (m_guardFlag == false)
			{
				int floorBossAttackPower = 0;
				floorBossAttackPower = m_floorBoss->GetAttackPower(floorBossAttackPower);
				m_playerHp -= floorBossAttackPower;
			}
			else if (m_guardFlag == true)
			{
				int floorBossAttackPower = 0;
				floorBossAttackPower = m_floorBoss->GetAttackPower(floorBossAttackPower);
				m_playerHp -= floorBossAttackPower / 2;
			}
			m_damageIntarvalTime = 1.0f;
		}
	}

	const auto& collisions4 = g_collisionObjectManager->FindCollisionObjects("finalBossAttack");
	for (auto collision : collisions4)
	{
		if (collision->IsHit(m_characterController) == true && m_damageIntarvalTime == 0.0f)
		{
			if (m_guardFlag == false)
			{
				int finalBossAttackPower = 0;
				finalBossAttackPower = m_finalBoss->GetAttackPower(finalBossAttackPower);
				m_playerHp -= finalBossAttackPower;
			}
			else if (m_guardFlag == true)
			{
				int finalBossAttackPower = 0;
				finalBossAttackPower = m_finalBoss->GetAttackPower(finalBossAttackPower);
				m_playerHp -= finalBossAttackPower / 2;
			}
			m_damageIntarvalTime = 1.0f;
		}
	}

	const auto& collisions5 = g_collisionObjectManager->FindCollisionObjects("powerBuffPotion");
	for (auto collision : collisions5)
	{
		if (collision->IsHit(m_characterController) == true)
		{
			m_powerBuffFlag = true;
			m_powerBuffTime = 20.0f;
		}
	}

	const auto& collisions6 = g_collisionObjectManager->FindCollisionObjects("attackSpeedBuffPotion");
	for (auto collision : collisions6)
	{
		if (collision->IsHit(m_characterController) == true)
		{
			m_attackSpeedBuffFlag = true;
			m_attackSpeedBuffTime = 20.0f;
		}
	}

	const auto& collisions7 = g_collisionObjectManager->FindCollisionObjects("healPotion");
	for (auto collision : collisions7)
	{
		if (collision->IsHit(m_characterController) == true)
		{
			Heal();
		}
	}

	const auto& collisions8 = g_collisionObjectManager->FindCollisionObjects("gireCollision");
	for (auto collision : collisions8)
	{
		if (collision->IsHit(m_characterController) == true)
		{
			m_isGetGire = true;
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

void Player::GuardCollision()
{
	m_collisionObject = NewGO<CollisionObject>(0);
	Vector3 collisionPos = m_position;
	m_rotation.Apply(m_forward);
	collisionPos += m_forward * 250.0f;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("playerGuard");
}

void Player::Gurad()
{
	if (g_pad[0]->IsPress(enButtonX))
	{
		m_guardFlag = true;
		//GuardCollision();
	}
	else
	{
		m_guardFlag = false;
	}
}

void Player::PlayerState()
{
	if(m_characterController.IsOnGround() == false)
	{
		m_playerState = 1;


		return;
	}

	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		m_playerState = 2;
		if (g_pad[0]->IsPress(enButtonY))
		{
			m_playerState = 3;
		}
	}

	else
	{
		m_playerState = 0;
	}
}

void Player::PlayAnimation()
{
	switch(m_playerState)
	{
	case 0:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case 1:
		m_modelRender.PlayAnimation(enAnimationClip_Jump);
		break;
	case 2:
		m_modelRender.PlayAnimation(enAnimationClip_Walk);
		break;
	case 3:
		m_modelRender.PlayAnimation(enAnimationClip_Run);
		break;
	default:
		break;
	}
}

void Player::PowerBuff()
{
	if (m_powerBuffFlag == true)
	{
		m_attackPower = 20;
		if (m_powerBuffTime <= 0)
		{
			m_powerBuffFlag == false;
		}
	}
	else
	{
		m_attackPower = 10;
	}
}

void Player::PowerBuffTime()
{
	m_powerBuffTime -= g_gameTime->GetFrameDeltaTime();
	if (m_powerBuffTime < 0.0f)
	{
		m_powerBuffTime = 0.0f;
		m_powerBuffFlag = false;
	}
}

void Player::AttackSpeedBuffTime()
{
	m_attackSpeedBuffTime -= g_gameTime->GetFrameDeltaTime();
	if (m_attackSpeedBuffTime < 0.0f)
	{
		m_attackSpeedBuffTime = 0.0f;
		m_attackSpeedBuffFlag = false;
	}
}

void Player::Heal()
{
	m_playerHp += m_heal;
	if (m_playerHp > m_playerMaxHp)
	{
		m_playerHp = m_playerMaxHp;
	}
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}