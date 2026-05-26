#include "stdafx.h"
#include "FinalBoss.h"
#include "Source/Actor/Character/Player/Player.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Game.h"
#include "Source/UIBase/DamageText/DamageText.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "Source/Actor/Bullet/Bullet.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/EffectManager/EffectManager.h"

FinalBoss::FinalBoss()
{
	
}

FinalBoss::~FinalBoss()
{
}

bool FinalBoss::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Enemy/finalBoss/finalBossIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Enemy/finalBoss/finalBossWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Shot].Load("Assets/animData/Enemy/finalBoss/finalBossShot.tka");
	m_animationClips[enAnimationClip_Shot].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Death].Load("Assets/animData/Enemy/finalBoss/finalBossDeath.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/Enemy/finalBoss/finalBoss.tkm", m_animationClips, enAnimationClip_Num);
	m_characterController.Init(300.0f, 100.0f, m_position);

	m_bossHPFrame.Init("Assets/UIData/enemyHPFrame.DDs", 1024, 128.0f);
	m_bossHPFrame.SetPosition(Vector3(10.5f, 450.0f, 0.0f));
	m_bossHPFrame.SetScale(Vector3(1.52f, 0.3f, 1.0f));
	m_bossHPFrame.Update();

	m_bossHPBar.Init("Assets/UIData/enemyHPBar.DDs", 1024, 128.0f);
	m_bossHPBar.SetPosition(Vector3(-760.0f, 447.0f, 0.0f));
	m_bossHPBar.SetScale(Vector3(1.48f, 0.3f, 1.0f));
	m_bossHPBar.SetPivot(Vector2(0.0f, 0.5f));
	m_bossHPBar.Update();

	m_player = FindGO<Player>("player");
	m_game = FindGO<Game>("game");
	m_audioManager = FindGO<AudioManager>("audioManager");
	m_difficultyLevel = FindGO<DifficultyLevel>("difficultyLevel");

	//========================
	// 難易度設定
	//========================
	if (m_game != nullptr)
	{
		switch (m_game->GetDifficulty())
		{
		case EASY:
			m_finalBossHp = 200;
			m_finalBossMaxHp = 200;
			m_attackPower = 10;
			m_shotCoolTimeReset = 3.0f;
			break;

		case NORMAL:
			m_finalBossHp = 300;
			m_finalBossMaxHp = 300;
			m_attackPower = 20;
			m_shotCoolTimeReset = 2.0f;
			break;

		case HARD:
			m_finalBossHp = 500;
			m_finalBossMaxHp = 500;
			m_attackPower = 35;
			m_shotCoolTimeReset = 1.5f;
			break;

		case LUNATIC:
			m_finalBossHp = 750;
			m_finalBossMaxHp = 750;
			m_attackPower = 50;
			m_shotCoolTimeReset = 1.0f;
			break;
		}
	}

	m_finalBossState = enFinalBossState_Idle;
	return true;
}

void FinalBoss::Update()
{
	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);

	if (isPause || m_game->IsGameStop())
	{
		return;
	}
	
	bool isFade = m_game->IsFade();
	bool IntroFlag = m_game->GetIntro();
	bool bossIntroFlag = m_game->GetBossIntro();

	if (!isFade &&
		!IntroFlag &&
		!bossIntroFlag)
	{
		Move();

		Rotation();

		Shot();

		PlayAnimation();

		ManageState();
	}

	if(m_collisionObject != nullptr)
	{
		m_attackCollisionLife -= g_gameTime->GetFrameDeltaTime();
		if (m_attackCollisionLife <= 0.0f)
		{
			DeleteGO(m_collisionObject);
			m_collisionObject = nullptr;
		}
	}

	if (m_shotCoolTime > 0.0f)
	{
		m_shotCoolTime -= g_gameTime->GetFrameDeltaTime();
	}
	Time();

	Hit();

	DamageIntarval();

	AttackHit();

	FinalBossHP();

	float dist = (m_player->GetPosition() - m_position).Length();

	if (dist <= 1500.0f)
	{
		m_isShowBossHP = true;
	}
	else
	{
		m_isShowBossHP = false;
	}

	m_modelRender.Update();
}

void FinalBoss::Move()
{
	if (m_finalBossState != enFinalBossState_Chase)
	{
		m_moveSpeed = Vector3::Zero;
		return;
	}

	Vector3 diff = m_player->GetPosition() - m_position;
	diff.y = 0.0f;

	if (diff.LengthSq() > 0.0001f)
	{
		diff.Normalize();
		m_moveSpeed.x = diff.x * 1000.0f;
		m_moveSpeed.z = diff.z * 1000.0f;
	}

	m_position = m_characterController.Execute(
		m_moveSpeed,
		g_gameTime->GetFrameDeltaTime()
	);

	m_modelRender.SetPosition(m_position);
}

void FinalBoss::Rotation()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	toPlayer.y = 0.0f;
	toPlayer.Normalize();

	m_forward = toPlayer;

	m_rotation.SetRotationYFromDirectionXZ(m_forward);
	m_modelRender.SetRotation(m_rotation);
}

void FinalBoss::Shot()
{
	if (m_shotCoolTime > 0.0f)
	{
		return;
	}

	if (m_finalBossState != enFinalBossState_Shot)
	{
		return;
	}

	// 一回だけ発射
	if (m_isShot)
	{
		return;
	}

	m_isShot = true;

	// 弾生成
	m_bullet = NewGO<Bullet>(0, "bossBullet");
	int randomNum = rand() % 2 + 1;
	AudioID id;
	switch (randomNum)
	{
	case 1: id = enSound_BossShotSE_01;
		break;
	case 2: id = enSound_BossShotSE_02;
		break;
	}

	m_audioManager->PlaySE(
		id,
		1.0f,
		enSEPlay_AllowOverlap
	);

	// 発射位置
	Vector3 shotPos = m_position;

	Vector3 forward = Vector3::Front;
	m_rotation.Apply(forward);

	shotPos += forward * 450.0f;
	shotPos.y += 450.0f;

	m_bullet->SetPosition(shotPos);
	m_bullet->SetStartPosition(shotPos);

	// 弾速度
	m_bullet->SetMoveSpeed(forward * 1500.0f);
	m_shotCoolTime = m_shotCoolTimeReset;
}

void FinalBoss::Hit()
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
				damage *= 2;
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
				m_finalBossHp -= damage;
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
				m_finalBossHp -= damage;
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
		}
	}
}

const bool FinalBoss::SearchPlayer() const
{
	Vector3 diff = m_player->GetPosition() - m_position;

	if (diff.LengthSq() <= 3000.0f * 3000.0f)
	{
		diff.Normalize();
		float cos = m_forward.Dot(diff);
		float angle = acosf(cos);
		if (angle <= (Math::PI / 180.0f) * 120.0f)
		{
			return true;
		}
	}
	return false;
}

void FinalBoss::DamageIntarval()
{
	m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_damageIntarvalTime < 0.0f)
	{
		m_damageIntarvalTime = 0.0f;
	}
}

void FinalBoss::Death()
{
	if (m_finalBossHp <= 0)
	{
		if (m_isDead)
		{
			return;
		}

		m_isDead = true;

		m_game->EnemyCount();

		MakeExplosionEffect();

		m_gire = NewGO<Gire>(0, "gire");

		m_gire->SetPosition(Vector3(m_position.x,m_position.y + 50.0f,m_position.z));

		m_gire->SetScale(Vector3(3.0f, 3.0f, 3.0f));

		DeleteGO(this);
	}
}

void FinalBoss::MakeExplosionEffect()
{
	//爆発エフェクトの生成
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;

	EffectManager::GetInstance().PlayEffect(
		EffectManager::enEffect_BossExplosion,
		effectPos,
		50.0f
	);
}

void FinalBoss::ManageState()
{
	switch (m_finalBossState)
	{
	case enFinalBossState_Idle:
		IdleState();
		break;
	case enFinalBossState_Chase:
		WalkState();
		break;
	case enFinalBossState_Shot:
		ShotState();
		break;
	case enFinalBossState_Death:
		DeathState();
		break;
	default:
		break;
	}
}

void FinalBoss::PlayAnimation()
{
	EnAnimationClip nextAnimationClip = enAnimationClip_Idle;

	switch (m_finalBossState)
	{
	case enFinalBossState_Idle:
		nextAnimationClip = enAnimationClip_Idle;
		break;
	case enFinalBossState_Chase:
		nextAnimationClip = enAnimationClip_Walk;
		break;
	case enFinalBossState_Shot:
		nextAnimationClip = enAnimationClip_Shot;
		break;
	case enFinalBossState_Death:
		nextAnimationClip = enAnimationClip_Death;
		break;
	default:
		break;
	}

	if(m_currentAnimationClip != nextAnimationClip)
	{
		m_modelRender.PlayAnimation(nextAnimationClip);
		m_currentAnimationClip = nextAnimationClip;
	}
}

void FinalBoss::FinalBossState()
{
	m_idleTimer = 0.0f;
	m_chaseTimer = 0.0f;

	Vector3 diff = m_player->GetPosition() - m_position;

	if (m_finalBossHp <= 0)
	{
		m_finalBossState = enFinalBossState_Death;
		return;
	}

	if (SearchPlayer())
	{
		float dist = (m_player->GetPosition() - m_position).Length();

		if (dist <= 2000.0f)
		{
			if (m_shotCoolTime <= 0.0f)
			{
				m_finalBossState = enFinalBossState_Shot;
			}
			else
			{
				m_finalBossState = enFinalBossState_Idle;
			}
		}
		else
		{
			m_finalBossState = enFinalBossState_Chase;
		}
	}
	else
	{
		m_finalBossState = enFinalBossState_Idle;
	}
}

void FinalBoss::IdleState()
{
	m_moveSpeed = Vector3::Zero;

	m_idleTimer += g_gameTime->GetFrameDeltaTime();

	if (m_idleTimer >= 0.9f)
	{
		FinalBossState();
	}
}

void FinalBoss::WalkState()
{
	float dist = (m_player->GetPosition() - m_position).Length();

	if (dist <= 2000.0f)
	{
		FinalBossState();
		return;
	}
}

void FinalBoss::ShotState()
{
	// 攻撃アニメ終了
	if (!m_modelRender.IsPlayingAnimation())
	{
		m_isShot = false;

		// 一旦Idleへ戻す
		m_finalBossState = enFinalBossState_Idle;
	}
}

void FinalBoss::DeathState()
{
	if(m_modelRender.IsPlayingAnimation() == false)
	{
		Death();
	}
}

const bool FinalBoss::IsCanAttack() const
{
	Vector3 diff = m_player->GetPosition() - m_position;
	if (diff.LengthSq() <= 300.0f * 300.0f)
	{
		return true;
	}
	return false;
}

void FinalBoss::FinalBossHP()
{
	float rate = (float)m_finalBossHp / (float)m_finalBossMaxHp;

	// 下限
	if (rate < 0.0f)
	{
		rate = 0.0f;
	}

	// 上限
	if (rate > 1.0f)
	{
		rate = 1.0f;
	}

	Vector3 scale = { 1.5f, 0.3f, 1.0f };
	scale.x *= rate;

	m_bossHPBar.SetScale(scale);

	if (m_finalBossHp <= m_finalBossMaxHp / 4)
	{
		m_bossHPBar.SetMulColor(g_vec4Red);
	}
	else
	{
		m_bossHPBar.SetMulColor(g_vec4Green);
	}

	m_bossHPBar.Update();
}

void FinalBoss::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);

	if (m_isShowBossHP)
	{
		m_bossHPFrame.Draw(rc);
		m_bossHPBar.Draw(rc);
	}
}