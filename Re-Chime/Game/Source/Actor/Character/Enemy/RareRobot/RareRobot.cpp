#include "stdafx.h"
#include "RareRobot.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "collision/CollisionObject.h"
#include "Game.h"
#include "Source/UIBase/DamageText/DamageText.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/EffectManager/EffectManager.h"

RareRobot::RareRobot()
{

}

RareRobot::~RareRobot()
{
}

bool RareRobot::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Enemy/rareRobot/rareRobotIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);

	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Enemy/rareRobot/rareRobotWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);

	m_animationClips[enAnimationClip_Death].Load("Assets/animData/Enemy/rareRobot/rareRobotDeath.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);

	m_modelRender.Init(
		"Assets/modelData/Enemy/rareRobot/rareRobot.tkm",
		m_animationClips,
		enAnimationClip_Num
	);

	m_characterController.Init(200.0f, 100.0f, m_position);

	m_enemyHP.Init("Assets/UIData/enemyHPBar.DDs", 1024.0f, 128.0f);
	m_enemyHP.SetPivot(Vector2(0.0f, 0.5f));
	m_enemyHP.Update();

	m_enemyHPFrame.Init("Assets/UIData/enemyHPFrame.DDs", 1024.0f, 128.0f);
	m_enemyHPFrame.SetScale(Vector3(0.29f, 0.29f, 0.0f));
	m_enemyHPFrame.SetPivot(Vector2(0.02f, 0.4f));
	m_enemyHPFrame.Update();

	m_audioManager = FindGO<AudioManager>("audioManager");

	m_player = FindGO<Player>("player");
	m_game = FindGO<Game>("game");

	//========================
	// 難易度設定
	//========================
	if (m_game != nullptr)
	{
		switch (m_game->GetDifficulty())
		{
		case EASY:
			m_rareRobotHp = 50;
			m_rareRobotMaxHp = 50;
			m_attackPower = 5;
			m_knockBackPower = 1200.0f;
			break;

		case NORMAL:
			m_rareRobotHp = 75;
			m_rareRobotMaxHp = 75;
			m_attackPower = 10;
			m_knockBackPower = 800.0f;
			break;

		case HARD:
			m_rareRobotHp = 120;
			m_rareRobotMaxHp = 120;
			m_attackPower = 15;
			m_knockBackPower = 500.0f;
			break;

		case LUNATIC:
			m_rareRobotHp = 180;
			m_rareRobotMaxHp = 180;
			m_attackPower = 25;
			m_knockBackPower = 200.0f;
			break;
		}
	}

	return true;
}
void RareRobot::Update()
{
	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);
	if (isPause || m_game->IsGameStop())
	{
		return;
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
	bool IntroFlag = m_game->GetIntro();
	bool bossIntroFlag = m_game->GetBossIntro();

	if (!m_isDeath && !IntroFlag && !bossIntroFlag)
	{
		KnockBack();

		if (!m_isKnockBack)
		{
			Move();
		}

		Rotation();
	}

	SearchPlayer();

	Time();

	Hit();

	Attack();

	DamageIntarval();

	AttackHit();

	ManageState();

	PlayAnimation();

	RareRobotHP();

	m_modelRender.Update();
}

void RareRobot::KnockBack()
{
	if (!m_isKnockBack)
	{
		return;
	}

	m_knockBackTime -= g_gameTime->GetFrameDeltaTime();

	m_position = m_characterController.Execute(
		m_knockBackMove,
		2.0f / 60.0f
	);

	m_modelRender.SetPosition(m_position);

	// 徐々に減速
	m_knockBackMove *= 0.90f;

	if (m_knockBackTime <= 0.0f)
	{
		m_isKnockBack = false;
		m_knockBackMove = Vector3::Zero;
	}
}

void RareRobot::Move()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 500 && m_timeCount == 0.0f)
	{
		m_timeCount = 2.0f;
		Time();
	}
	if (distToPlayer <= 1000)
	{
		toPlayer.Normalize();
		m_moveSpeed = toPlayer * 100.0f;
		m_moveSpeed.y = 0.0f;
	}
	else if (distToPlayer > 1000)
	{
		m_moveSpeed = toPlayer * 0.0f;
	}

	if (m_characterController.IsOnGround() == false)
	{
		//m_moveSpeed.y -= 40.0f;
	}

	m_position = m_characterController.Execute(m_moveSpeed, 2.0f / 60.0f);

	m_modelRender.SetPosition(m_position);
}

void RareRobot::Rotation()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 1000)
	{
		toPlayer.Normalize();
		m_rotation.SetRotationYFromDirectionXZ(toPlayer);
	}
	m_modelRender.SetRotation(m_rotation);
}

void RareRobot::SearchPlayer()
{
	m_searchPlayer = false;

	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);

	Vector3 playerPos = m_player->GetPosition();
	Vector3 diff = playerPos - m_position;

	if (diff.Length() <= 2000.0f)
	{
		diff.Normalize();
		float angle = acosf(diff.Dot(m_forward));
		if (Math::PI * 0.15f <= fabsf(angle))
		{
			return;
		}
		m_searchPlayer = true;
	}
}


void RareRobot::Attack()
{
	if (m_isAttack == false)
	{
		return;
	}
	else
	{
		OnCollision();
		m_isAttack = false;
	}
	//OnCollision();
}

void RareRobot::OnCollision()
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
	collisionPos += m_forward * 250.0f;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("rareRobotAttack");

	m_attackCollisionLife = 0.1f; // 攻撃の当たり判定の寿命を設定
}

void RareRobot::Time()
{
	m_timeCount -= g_gameTime->GetFrameDeltaTime();
	if (m_timeCount < 0.0f)
	{
		m_timeCount = 0.0f;
	}
}

void RareRobot::Hit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("playerAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_characterController) == true && m_damageIntarvalTime == 0.0f)
		{
			int damage = 0;
			damage = m_player->GetAttackPower();
			int randomNum = rand() % 100 + 1;
			if (randomNum <= 5)
			{
				if (!m_searchPlayer)
				{
					damage *= 3;
					bool isHit = m_player->GetAttackHit();
					if (!isHit)
					{
						m_player->SetAttackHit(true);

						m_audioManager->PlaySE(
							enSound_BackstabSE,
							1.0f,
							enSEPlay_AllowOverlap
						);
					}
				}
				else
				{
					damage *= 1.5;
					bool isHit = m_player->GetAttackHit();
					if (!isHit)
					{
						m_player->SetAttackHit(true);

						m_audioManager->PlaySE(
							enSound_CriticalSE,
							1.0f,
							enSEPlay_AllowOverlap
						);
					}
				}

				m_rareRobotHp -= damage;
				m_searchPlayer = true;

				//========================
				// ノックバック
				//========================
				Vector3 dir = m_position - m_player->GetPosition();

				dir.y = 0.0f;

				if (dir.LengthSq() > 0.001f)
				{
					dir.Normalize();
				}

				m_knockBackMove = dir * m_knockBackPower;

				m_isKnockBack = true;

				m_knockBackTime = 0.2f;
			}
			else
			{
				if (!m_searchPlayer)
				{
					damage *= 1.5;
					bool isHit = m_player->GetAttackHit();
					if (!isHit)
					{
						m_player->SetAttackHit(true);

						m_audioManager->PlaySE(
							enSound_BackstabSE,
							1.0f,
							enSEPlay_AllowOverlap
						);
					}
				}
				else
				{
					damage *= 1;
					bool isHit = m_player->GetAttackHit();
					if (!isHit)
					{
						m_player->SetAttackHit(true);
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
					}
				}
				m_rareRobotHp -= damage;
				m_searchPlayer = true;

				//========================
				// ノックバック
				//========================
				Vector3 dir = m_position - m_player->GetPosition();

				dir.y = 0.0f;

				if (dir.LengthSq() > 0.001f)
				{
					dir.Normalize();
				}

				m_knockBackMove = dir * m_knockBackPower;

				m_isKnockBack = true;

				m_knockBackTime = 0.2f;
			}
			m_damageIntarvalTime = 1.5f;


			//========================
			// ダメージ表示生成
			//========================
			DamageText* damageText = NewGO<DamageText>(0);

			Vector3 textPos = m_position;

			textPos.y += 250.0f;

			damageText->SetPosition(textPos);

			damageText->SetDamage(damage);
			m_damageIntarvalTime = 2.0f;
		}
	}
}

void RareRobot::AttackHit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("rareRobotAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_player->GetCharacterController()) == true)
		{
			int rareRobotAttackPower = 0;
			rareRobotAttackPower = GetAttackPower();
			m_player->TakeDamage(rareRobotAttackPower, m_position);
		}
	}
}

void RareRobot::DamageIntarval()
{
	m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_damageIntarvalTime < 0.0f)
	{
		m_damageIntarvalTime = 0.0f;
	}
}

void RareRobot::Death()
{
	MakeExplosionEffect();
	m_audioManager->PlaySE(enSound_EnemyDeathSE, 0.5f, enSEPlay_AllowOverlap);
	int randomNum = rand() % 3 + 1;
	if (randomNum == 1)
	{
		m_attackSpeedBuff = NewGO<AttackSpeedBuff>(0);
		m_attackSpeedBuff->SetPosition(m_position);
		m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
	}
	else if (randomNum == 2)
	{
		m_powerBuff = NewGO<PowerBuff>(0);
		m_powerBuff->SetPosition(m_position);
		m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
	}
	else if (randomNum == 3)
	{
		m_heal = NewGO<Heal>(0);
		m_heal->SetPosition(m_position);
		m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
	}
	DeleteGO(this);
}

void RareRobot::MakeExplosionEffect()
{
	//爆発エフェクトの生成
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;

	EffectManager::GetInstance().PlayEffect(
		EffectManager::enEffect_Explosion,
		effectPos,
		50.0f
	);
}

void RareRobot::ManageState()
{
	switch (m_rareRobotState)
	{
	case enRareRobotState_Idle:
		IdleState();
		break;
	case enRareRobotState_Walk:
		WalkState();
		break;
	case enRareRobotState_Death:
		DeathState();
		break;
	case enRareRobotState_Attack:
		AttackState();
		break;
	default:
		break;
	}
}

void RareRobot::PlayAnimation()
{
	switch (m_rareRobotState)
	{
	case enRareRobotState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case enRareRobotState_Walk:
		m_modelRender.PlayAnimation(enAnimationClip_Walk);
		break;
	case enRareRobotState_Death:
		m_modelRender.PlayAnimation(enAnimationClip_Death);
		break;
	default:
		break;
	}
}

void RareRobot::RareRobotState()
{
	if (m_timeCount == 0 && !m_isDeath)
	{
		m_rareRobotState = enRareRobotState_Attack;
		m_isAttack = true;
	}
	if (m_rareRobotHp <= 0)
	{
		m_rareRobotState = enRareRobotState_Death;
		m_isDeath = true;
	}
	else if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		m_rareRobotState = enRareRobotState_Walk;
	}
	else
	{
		m_rareRobotState = enRareRobotState_Idle;
	}
}

void RareRobot::IdleState()
{
	RareRobotState();
}

void RareRobot::WalkState()
{
	RareRobotState();
}

void RareRobot::AttackState()
{
	RareRobotState();
}

void RareRobot::DeathState()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		Death();
	}
}

Vector3 RareRobot::GetPosition()const
{
	return m_position;
}

void RareRobot::RareRobotHP()
{
	float dist = (m_player->GetPosition() - m_position).Length();
	if (dist > 1000.0f)
	{
		m_isShowHP = false;
		return;
	}

	m_isShowHP = true;

	int nowHP = 0;
	int MaxHP = 0;

	nowHP = GetHP();
	MaxHP = m_rareRobotMaxHp;
	float Wari = (float)nowHP / (float)MaxHP;
	Vector3 scale = { 0.28f,0.28f,0.5f };
	scale.x *= Wari;
	m_enemyHP.SetScale(scale);
	if (nowHP <= MaxHP / 4)
	{
		m_enemyHP.SetMulColor(g_vec4Red);
	}
	else
	{
		m_enemyHP.SetMulColor(g_vec4Green);
	}

	//HPの位置の調整
	Vector3 hpPos = m_position;
	hpPos.y += 450.0f;

	g_camera3D->CalcScreenPositionFromWorldPosition(m_enemyHPBarPosition, hpPos);
	m_enemyHP.SetPosition(Vector3(m_enemyHPBarPosition.x, m_enemyHPBarPosition.y, 0.0f));
	m_enemyHP.Update();

	g_camera3D->CalcScreenPositionFromWorldPosition(m_enemyHPFramePosition, hpPos);
	m_enemyHPFrame.SetPosition(Vector3(m_enemyHPFramePosition.x, m_enemyHPFramePosition.y, 0.0f));
	m_enemyHPFrame.Update();
}

void RareRobot::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	if (m_isShowHP)
	{
		m_enemyHPFrame.Draw(rc);
		m_enemyHP.Draw(rc);
	}
}
