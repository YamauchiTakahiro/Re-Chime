#include "stdafx.h"
#include "Player.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Game.h"
#include "Source/Actor/Character/Enemy/SmallRobot/SmallRobot.h"
#include "Source/Actor/Character/Enemy/MediumRobot/MediumRobot.h"
#include "Source/Actor/Character/Enemy/FloorBoss/FloorBoss.h"
#include "Source/Actor/Character/Enemy/FinalBoss/FinalBoss.h"
#include "Source/Sound/AudioManager/AudioManager.h"

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
	m_game = FindGO<Game>("game");	
	m_smallRobot = FindGO<SmallRobot>("smallRobot");
	m_mediumRobot = FindGO<MediumRobot>("mediumRobot");
	m_floorBoss = FindGO<FloorBoss>("FloorBoss");
	m_finalBoss = FindGO<FinalBoss>("finalBoss");

	m_audioManager = FindGO<AudioManager>("audioManager");

	m_modelRender.SetShadowCasterFlag(true);

	SetScale();

	return true;
}

void Player::Update()
{
	m_gire = FindGO<Gire>("gire");
	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);
	if (isPause)
	{
		return;
	}

	bool fadeFlag = m_game->FadeFlag();

	if(fadeFlag)
	{
		m_fadeTime = 1.0f;
	}

	if (!m_guardFlag && !m_isAttack && m_fadeTime <= 0)
	{
		Move();

		Rotation();
	}

	if (m_collisionObject != nullptr)
	{
		m_attackCollisionLife -= g_gameTime->GetFrameDeltaTime();
		if (m_attackCollisionLife <= 0.0f)
		{
			DeleteGO(m_collisionObject);
			m_collisionObject = nullptr;
		}
	}

	FadeTime();

	FootStepTime();

	Time();

	Hit();

	GetGires();

	DamageIntarval();

	GuradInterval();

	PowerBuff();

	PowerBuffTime();

	AttackSpeedBuffTime();

	Attack();

	ManageState();

	FootStep();

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

		if(moveDir.LengthSq() > 0.0001f)
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
	
	if (m_characterController.IsOnGround())
	{
		m_moveSpeed.y = 0.0f;
		if (m_isJump)
		{
			JumpTime();
			if (m_jumpTime == 0.0f)
			{
				m_moveSpeed.y = 500.0f;
				m_jumpTime = 1.0f;
			}
		}
	}
	if (m_characterController.IsOnGround() == false)
	{
		m_moveSpeed.y -= 20.0f;
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

void Player::FootStep()
{
	if (!m_characterController.IsOnGround())
	{
		return;
	}

	switch (m_playerState)
	{
	case enPlayerState_Walk:

		if (m_footStepTime <= 0.0f)
		{
			printf("walk footstep\n");

			// 歩き足音をランダム再生
			int r = rand() % 3;

			AudioID id;

			switch (r)
			{
			case 0: id = enSound_PlayerWalkSE1; break;
			case 1: id = enSound_PlayerWalkSE2; break;
			case 2: id = enSound_PlayerWalkSE3; break;
			}

			m_audioManager->PlaySE(
				id,
				1.0f,
				enSEPlay_AllowOverlap
			);

			m_footStepTime = 0.5f;
		}

		break;

	case enPlayerState_Run:

		if (m_footStepTime <= 0.0f)
		{
			printf("run footstep\n");

			// ダッシュ足音をランダム再生
			int r = rand() % 3;

			AudioID id;

			switch (r)
			{
			case 0: id = enSound_PlayerDashSE1; break;
			case 1: id = enSound_PlayerDashSE2; break;
			case 2: id = enSound_PlayerDashSE3; break;
			}
			m_audioManager->PlaySE(
				id,
				1.0f,
				enSEPlay_AllowOverlap
			);

			m_footStepTime = 0.3f;
		}

		break;

	default:
		break;
	}
}
void Player::Attack()
{
	if (m_playerState != enPlayerState_Attack)
	{
		return;
	}

	if (m_isAttack && !m_isKnockBack)
	{
		// 攻撃開始からの時間を計測
		m_attackStartTime += g_gameTime->GetFrameDeltaTime();

		// 0.2秒後に当たり判定生成
		if (m_attackStartTime >= 0.2f &&
			!m_hasCreatedAttackCollision)
		{
			OnCollision();

			m_hasCreatedAttackCollision = true;

			if (m_attackSpeedBuffFlag)
			{
				m_timeCount = 1.0f;
			}
			else
			{
				m_timeCount = 2.0f;
			}			
		}
		if (m_attackStartTime >= 0.25f &&
			!m_hasPlayedHitSE)
		{
			if (m_enemyHitFlag)
			{
				// 敵に攻撃が当たった場合の処理
				int r = rand() % 3;

				AudioID id;

				switch (r)
				{
				case 0: id = enSound_PlayerAttackSE_01; break;
				case 1: id = enSound_PlayerAttackSE_02; break;
				case 2: id = enSound_PlayerAttackSE_03; break;
				}

				m_audioManager->PlaySE(
					id,
					1.0f,
					enSEPlay_AllowOverlap
				);
				m_hasPlayedHitSE = true;
			}
			if (!m_enemyHitFlag)
			{
				int r = rand() % 3;

				AudioID id;

				switch (r)
				{
				case 0: id = enSound_MissSE_01; break;
				case 1: id = enSound_MissSE_02; break;
				case 2: id = enSound_MissSE_03; break;
				}

				m_audioManager->PlaySE(
					id,
					1.0f,
					enSEPlay_AllowOverlap
				);
				m_audioManager->PlaySE(id, 1.0f, enSEPlay_AllowOverlap);
				m_audioManager->PlaySE(id, 1.0f, enSEPlay_AllowOverlap);
				m_audioManager->PlaySE(id, 1.0f, enSEPlay_AllowOverlap);
				m_hasPlayedHitSE = true;
			}
		}
	}
}

void Player::OnCollision()
{
	if (m_collisionObject != nullptr)
	{
		DeleteGO(m_collisionObject);
		m_collisionObject = nullptr;
	}
	m_collisionObject = NewGO<CollisionObject>(0);

	Vector3 collisionPos = m_position;
	m_forward = Vector3::Front;
	m_rotation.Apply(m_forward);
	collisionPos += m_forward * 300.0f;
	collisionPos.y += 50.0f; // 攻撃の当たり判定を少し上にずらす
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("playerAttack");

	m_attackCollisionLife = 0.1f;
}

void Player::Time()
{
	m_timeCount -= g_gameTime->GetFrameDeltaTime();
	if (m_timeCount < 0.0f)
	{
		m_timeCount = 0.0f;
	}
}

void Player::FadeTime()
{
	m_fadeTime -= g_gameTime->GetFrameDeltaTime();
	if (m_fadeTime < 0.0f)
	{
		m_fadeTime = 0.0f;
	}
}

void Player::TakeDamage(int damage, const Vector3& enemyPos)
{
	if (m_damageIntarvalTime > 0.0f)
	{
		return;
	}
	else
	{
		if (!m_guardFlag && m_damageIntarvalTime == 0.0f)
		{
			m_playerHp -= damage;

			// ノックバックの計算
			Vector3 dir = m_position - enemyPos;
			dir.y = 0.0f; // 水平方向のみにノックバックを適用
			dir.Normalize();

			m_knockBack = dir * 500.0f; // ノックバックの強さを調整
			m_knockBack.y = 0.0f; // ノックバックの垂直成分をゼロにする

			m_isKnockBack = true;
			m_damageIntarvalTime = 2.0f; // ダメージのインターバルを設定
		}
		else if (m_guardFlag && m_damageIntarvalTime == 0.0f)
		{
			m_playerHp -= damage / 2; // ガードしている場合はダメージを半減
			m_damageIntarvalTime = 2.0f; // ダメージのインターバルを設定
		}
	}

}

void Player::Hit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("powerBuffPotion");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_characterController) == true)
		{
			m_powerBuffFlag = true;
			MakePowerBuffEffect();
			m_audioManager->PlaySE(enSound_BuffSE, 1.0f, enSEPlay_AllowOverlap);
			m_powerBuffTime = 20.0f;
		}
	}

	const auto& collisions2 = g_collisionObjectManager->FindCollisionObjects("attackSpeedBuffPotion");
	for (auto collision : collisions2)
	{
		if (collision->IsHit(m_characterController) == true)
		{
			m_attackSpeedBuffFlag = true;
			MakeAttackSpeedBuffEffect();
			m_audioManager->PlaySE(enSound_BuffSE, 1.0f, enSEPlay_AllowOverlap);
			m_attackSpeedBuffTime = 20.0f;
		}
	}

	const auto& collisions3 = g_collisionObjectManager->FindCollisionObjects("healPotion");
	for (auto collision : collisions3)
	{
		if (collision->IsHit(m_characterController) == true)
		{
			Heal();
			m_isHealFlag = true;
		}
	}
}

void Player::GetGires()
{
	if(m_gire == nullptr)
	{
		return;
	}
	else if (m_gire != nullptr)
	{
		Vector3 diff = m_gire->GetPosition() - m_position;
		if (diff.LengthSq() < 250.0f * 250.0f)
		{
			if (g_pad[0]->IsTrigger(enButtonA))
			{
				m_isGetGire = true;
			}
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

void Player::JumpTime()
{
	m_jumpTime -= g_gameTime->GetFrameDeltaTime();
	if (m_jumpTime < 0.0f)
	{
		m_jumpTime = 0.0f;
	}
}

void Player::FootStepTime()
{
	m_footStepTime -= g_gameTime->GetFrameDeltaTime();
	if (m_footStepTime < 0.0f)
	{
		m_footStepTime = 0.0f;
	}
}

void Player::PlayerState()
{
	if (m_isKnockBack)
	{
		m_playerState = enPlayerState_KnockBack;
		return;
	}

	if (g_pad[0]->IsTrigger(enButtonA) && m_timeCount == 0.0f && !m_guardFlag)
	{
		m_playerState = enPlayerState_Attack;

		m_isAttack = true;

		// 追加
		m_attackStartTime = 0.0f;
		m_hasCreatedAttackCollision = false;

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

	if(m_isKnockBack == false && g_pad[0]->IsTrigger(enButtonB))
	{
		m_playerState = enPlayerState_Jump;
		m_isJump = true;
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
	if (m_modelRender.IsPlayingAnimation() == false || m_isKnockBack)
	{
		m_isAttack = false;
		
		// 追加
		m_hasCreatedAttackCollision = false;
		m_attackStartTime = 0.0f;
		m_enemyHitFlag = false;
		m_hasPlayedHitSE = false;

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
		m_isJump = false;
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
		m_audioManager->StopSE(enSound_PlayerGuardSE);
		m_audioManager->StopSE(enSound_PlayerDamageSE);
		IdleState();
		break;
	case enPlayerState_Jump:\
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
	if (m_fadeTime > 0.0f)
	{
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		return;
	}

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
	int attakcPower = rand() % 5 + 15; // 攻撃力を5から10の範囲でランダムに決定
	if (m_powerBuffFlag == true)
	{
		m_attackPower = attakcPower * 2;
		if (m_powerBuffTime <= 0)
		{
			m_powerBuffFlag = false;
		}
	}
	else
	{
		m_attackPower = attakcPower;
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
	m_audioManager->PlaySE(enSound_HealSE, 1.0f, enSEPlay_AllowOverlap);
	MakeHealEffect();
	if (m_playerHp > m_playerMaxHp)
	{
		m_playerHp = m_playerMaxHp;
	}
}

void Player::MakeHealEffect()
{
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(1);
	effectEmitter->SetScale(Vector3::One * 50.0f);
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;
	effectEmitter->SetPosition(effectPos);
	effectEmitter->Play();
}

void Player::MakePowerBuffEffect()
{
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(2);
	effectEmitter->SetScale(Vector3::One * 10.0f);
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;
	effectEmitter->SetPosition(effectPos);
	effectEmitter->Play();
}

void Player::MakeAttackSpeedBuffEffect()
{
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(3);
	effectEmitter->SetScale(Vector3::One * 10.0f);
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;
	effectEmitter->SetPosition(effectPos);
	effectEmitter->Play();
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