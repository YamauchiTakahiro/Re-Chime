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
	m_animationClips[enAnimationClip_Jump].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/Player/playerRun.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/Player/playerPunchRight.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Guard].Load("Assets/animData/Player/playerGuard.tka");
	m_animationClips[enAnimationClip_Guard].SetLoopFlag(false);
	m_animationClips[enAnimationClip_KnockBack].Load("Assets/animData/Player/playerKnockBack.tka");
	m_animationClips[enAnimationClip_KnockBack].SetLoopFlag(false);
	//モデルを初期化する。
	m_modelRender.Init("Assets/modelData/Player/player.tkm", m_animationClips, enAnimationClip_Num);
	m_characterController.Init(100.0f, 300.0f, m_position);
	m_gire = FindGO<Gire>("gire");
	m_game = FindGO<Game>("game");	
	const auto smallRobots = m_smallRobot = FindGO<SmallRobot>("smallRobot");
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

	Time();

	Hit();

	DamageIntarval();

	GuradInterval();

	PowerBuff();

	PowerBuffTime();

	AttackSpeedBuffTime();

	Attack();

	ManageState();

	PlayAnimation();
	m_modelRender.Update();
}

void Player::Move()
{
	if (!m_isKnockBack)
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

		Vector3 moveDir = forward * stickL.y + right * stickL.x;

		if(moveDir.LengthSq() > 0.0f > 0.0001f)
		{
			moveDir.Normalize();
		}

		if (g_pad[0]->IsPress(enButtonY) == false)
		{
			m_speed = 240.0f;
		}

		if (g_pad[0]->IsPress(enButtonY))
		{
			m_speed = 480.0f;
		}

		m_moveSpeed += moveDir * m_speed;
	}

	Vector3 finalMoveSpeed = m_moveSpeed;

	finalMoveSpeed.y = m_moveSpeed.y;

	if(m_isKnockBack)
	{
		finalMoveSpeed.x += m_knockBack.x;
		finalMoveSpeed.z += m_knockBack.z;

		m_knockBack *= 0.9f;

		if(m_knockBack.LengthSq() < 10.0f)
		{
			m_knockBack = Vector3::Zero;
			m_isKnockBack = false;
		}
	}

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

	m_position = m_characterController.Execute(finalMoveSpeed, 2.0f / 60.0f);

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
	if (m_playerState != enPlayerState_Attack)
	{
		return;
	}
	if (m_isAttack == true)
	{
		if(m_attackSpeedBuffFlag == true)
		{
			OnCollision();
			m_timeCount = 1.0f; // 攻撃クールタイムを短縮する例
		}
		else if (m_attackSpeedBuffFlag == false)
		{
			OnCollision();
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

void Player::TakeDamage(int damage, const Vector3& enemyPos)
{
	if (m_damageIntarvalTime > 0.0f)
	{
		return;
	}
	if (!m_guardFlag)
	{
		m_playerHp -= damage;

		// ノックバックの計算
		Vector3 dir = m_position - enemyPos;
		dir.y = 0.0f; // 水平方向のみにノックバックを適用
		dir.Normalize();

		m_knockBack = dir * 500.0f; // ノックバックの強さを調整
		m_knockBack.y = 0.0f; // ノックバックの垂直成分をゼロにする

		m_isKnockBack = true;
	}
	else
	{
		m_playerHp -= damage / 2; // ガードしている場合はダメージを半減
	}

	m_damageIntarvalTime = 1.0f; // ダメージのインターバルを設定
}

void Player::Hit()
{
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

void Player::GuradInterval()
{
	m_guardIntervalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_guardIntervalTime < 0.0f)
	{
		m_guardIntervalTime = 0.0f;
	}
	if (m_guardIntervalTime == 0.0f)
	{
		m_guardTimeLimit = 3.0f;
	}
}

void Player::GuradTimeLimit()
{
	m_guardTimeLimit -= g_gameTime->GetFrameDeltaTime();
	if (m_guardTimeLimit < 0.0f)
	{
		m_guardTimeLimit = 0.0f;
	}
}

void Player::PlayerState()
{
	if (m_isKnockBack)
	{
		m_playerState = enPlayerState_KnockBack;
		return;
	}

	if (g_pad[0]->IsTrigger(enButtonA))
	{
		m_playerState = enPlayerState_Attack;
		m_isAttack = true;
		return;
	}

	if(g_pad[0]->IsPress(enButtonX) && m_guardTimeLimit > 0)
	{
		m_playerState = enPlayerState_Guard;
		m_guardFlag = true;
		GuradTimeLimit();
		m_guardIntervalTime = 3.0f;
		return;
	}
	else
	{
		m_guardFlag = false;
	}

	if(m_isKnockBack == false && m_characterController.IsOnGround() == false)
	{
		m_playerState = enPlayerState_Jump;

		return;
	}

	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		if (g_pad[0]->IsPress(enButtonY))
		{
			m_playerState = enPlayerState_Run;
			return;
		}
		else
		{
			m_playerState = enPlayerState_Walk;
			return;
		}
	}

	else
	{
		m_playerState = enPlayerState_Idle;
		return;
	}
}

void Player::AttackState()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		PlayerState();
	}
}

void Player::IdleState()
{
	PlayerState();
}

void Player::WalkState()
{
	PlayerState();
}

void Player::RunState()
{
	PlayerState();
}

void Player::JumpState()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		PlayerState();
	}
}

void Player::GuardState()
{
	PlayerState();
}

void Player::KnockBackState()
{
	if (!m_isKnockBack)
	{
		PlayerState();
	}
}

void Player::ManageState()
{
	switch (m_playerState)
	{
	case enPlayerState_Idle:
		IdleState();
		break;
	case enPlayerState_Jump:
		JumpState();
		break;
	case enPlayerState_Walk:
		WalkState();
		break;
	case enPlayerState_Run:
		RunState();
		break;
	case enPlayerState_Attack:
		AttackState();
		break;
	case enPlayerState_Guard:
		GuardState();
		break;
	case enPlayerState_KnockBack:
		KnockBackState();
		break;
	default:
		break;
	}
}

void Player::PlayAnimation()
{
	switch(m_playerState)
	{
	case enPlayerState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case enPlayerState_Jump:
		m_modelRender.PlayAnimation(enAnimationClip_Jump);
		break;
	case enPlayerState_Walk:
		m_modelRender.PlayAnimation(enAnimationClip_Walk);
		break;
	case enPlayerState_Run:
		m_modelRender.PlayAnimation(enAnimationClip_Run);
		break;
	case enPlayerState_Attack:
		m_modelRender.PlayAnimation(enAnimationClip_Attack);
		break;
	case enPlayerState_Guard:
		m_modelRender.PlayAnimation(enAnimationClip_Guard);
		break;
	case enPlayerState_KnockBack:
		m_modelRender.PlayAnimation(enAnimationClip_KnockBack);
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
			m_powerBuffFlag = false;
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

const CharacterController& Player::GetCharacterController() const
{
	return m_characterController;
}

CharacterController& Player::GetCharacterController()
{
	return m_characterController;
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}