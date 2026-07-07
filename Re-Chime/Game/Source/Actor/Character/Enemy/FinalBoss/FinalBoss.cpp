#include "stdafx.h"
#include "FinalBoss.h"
#include "Source/Actor/Character/Player/Player.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Game.h"
#include "Source/UIBase/DamageText/DamageText.h"
#include "Source/Manager/AudioManager/AudioManager.h"
#include "Source/Actor/Bullet/Bullet.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/Manager/EffectManager/EffectManager.h"
#include "Source/Camera/GameCamera.h"

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

	m_alertMark.Init("Assets/UIData/AlertMark.DDs", 128.0f, 128.0f);
	m_alertMark.SetScale(Vector3(0.7f, 0.7f, 1.0f));
	m_alertMark.Update();

	m_questionMark.Init("Assets/UIData/QuestionMark.DDS", 128.0f, 128.0f);
	m_questionMark.SetScale(Vector3(0.7f, 0.7f, 1.0f));
	m_questionMark.Update();

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
			m_moveSpeedValue = 700.0f;
			m_searchRange = 3000.0f;
			m_bulletSpeed = 1200.0f;
			break;

		case NORMAL:
			m_finalBossHp = 300;
			m_finalBossMaxHp = 300;
			m_attackPower = 20;
			m_shotCoolTimeReset = 2.0f;
			m_moveSpeedValue = 850.0f;
			m_searchRange = 3500.0f;
			m_bulletSpeed = 1400.0f;
			break;

		case HARD:
			m_finalBossHp = 500;
			m_finalBossMaxHp = 500;
			m_attackPower = 35;
			m_shotCoolTimeReset = 1.5f;
			m_moveSpeedValue = 1000.0f;
			m_searchRange = 4000.0f;
			m_bulletSpeed = 1600.0f;
			break;

		case LUNATIC:
			m_finalBossHp = 750;
			m_finalBossMaxHp = 750;
			m_attackPower = 50;
			m_shotCoolTimeReset = 1.0f;
			m_moveSpeedValue = 1600.0f;
			m_searchRange = 4500.0f;
			m_bulletSpeed = 2000.0f;
			break;
		}
	}

	m_finalBossState = enFinalBossState_Idle;
	m_startPosition = m_position;
	m_viewHp = (float)m_finalBossHp;
	return true;
}

void FinalBoss::Update()
{
	UpdateHitStop();

	if (IsHitStop())
	{
		m_modelRender.Update();
		FinalBossHP();
		return;
	}

	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);

	if (isPause || m_game->IsGameStop())
	{
		return;
	}
	
	bool isFade = m_game->IsFade();
	bool IntroFlag = m_game->GetIntro();
	bool bossIntroFlag = m_game->GetBossIntro();

	if (m_isLostWaiting)
	{
		m_lostWaitTime -= g_gameTime->GetFrameDeltaTime();

		if (m_lostWaitTime <= 0.0f)
		{
			m_lostWaitTime = 0.0f;
			m_isLostWaiting = false;
		}
	}

	if (!isFade && !IntroFlag && !bossIntroFlag)
	{
		FinalBossState();

		ManageState();

		Move();

		Rotation();

		Shot();

		PlayAnimation();
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

	if (!m_firstPhaseChange &&
		m_finalBossHp <= m_finalBossMaxHp * 0.5f)
	{
		m_firstPhaseChange = true;

		m_shotCoolTimeReset *= 0.8f;
	}

	if (!m_secondPhaseChange &&
		m_finalBossHp <= m_finalBossMaxHp * 0.25f)
	{
		m_secondPhaseChange = true;

		m_shotCoolTimeReset *= 0.7f;
	}

	Time();

	Hit();

	DamageIntarval();

	AttackHit();

	FinalBossHP();

	if (m_isShowAlert)
	{
		m_alertTime -= g_gameTime->GetFrameDeltaTime();

		if (m_alertTime <= 0.0f)
		{
			m_isShowAlert = false;
		}
	}

	if (m_isShowAlert)
	{
		Vector3 alertPos = m_position;
		alertPos.y += 700.0f;

		Vector2 screenPos;
		g_camera3D->CalcScreenPositionFromWorldPosition(screenPos, alertPos);
		m_alertMark.SetPosition(Vector3(screenPos.x, screenPos.y, 0.0f));
		float dt = g_gameTime->GetFrameDeltaTime();
		m_alertScale += 5.0f * dt;
		if (m_alertScale > 0.3f)
		{
			m_alertScale = 0.3f;
		}
		m_alertMark.SetScale(Vector3(m_alertScale, m_alertScale, 1.0f));
		m_alertMark.Update();
	}

	if (m_isShowQuestion)
	{
		m_questionTime -= g_gameTime->GetFrameDeltaTime();

		if (m_questionTime <= 0.0f)
		{
			m_isShowQuestion = false;
		}
	}

	if (m_isShowQuestion)
	{
		Vector3 pos = m_position;
		pos.y += 700.0f;

		Vector2 screenPos;
		g_camera3D->CalcScreenPositionFromWorldPosition(screenPos, pos);

		m_questionMark.SetPosition(Vector3(screenPos.x, screenPos.y, 0.0f));

		float dt = g_gameTime->GetFrameDeltaTime();

		m_questionScale += 5.0f * dt;

		if (m_questionScale > 0.3f)
		{
			m_questionScale = 0.3f;
		}

		m_questionMark.SetScale(Vector3(m_questionScale, m_questionScale, 1.0f));
		m_questionMark.Update();
	}

	float dist = (m_player->GetPosition() - m_position).Length();

	m_isShowBossHP = !isFade && !IntroFlag && !bossIntroFlag && dist <= 4000.0f;

	m_modelRender.Update();
}

void FinalBoss::Move()
{
	if (m_isLostWaiting)
	{
		m_moveSpeed = Vector3::Zero;
		return;
	}

	float dist = (m_player->GetPosition() - m_position).Length();

	if (dist <= 2000.0f && !m_isReposition)
	{
		m_moveSpeed = Vector3::Zero;
		return;
	}

	Vector3 targetPos;

	if (m_isReposition)
	{
		targetPos = m_moveTarget;
	}
	// プレイヤーを発見中
	else if (m_hasDetectedPlayer)
	{
		targetPos = m_player->GetPosition();
	}
	// 見失ったら元の位置へ戻る
	else
	{
		targetPos = m_startPosition;
	}

	Vector3 diff = targetPos - m_position;
	diff.y = 0.0f;

	if (diff.LengthSq() > 50.0f * 50.0f)
	{
		diff.Normalize();

		float speed = m_moveSpeedValue;

		if (m_isReposition)
		{
			speed *= 1.8f;    // 撃った後だけ1.8倍
		}

		m_moveSpeed.x = diff.x * speed;
		m_moveSpeed.z = diff.z * speed;
	}
	else
	{
		m_moveSpeed = Vector3::Zero;

		if (m_isReposition)
		{
			m_isReposition = false;
		}
	}

	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	m_modelRender.SetPosition(m_position);
}

void FinalBoss::Rotation()
{
	// 探索モーション中
	if (m_isSearching)
	{
		m_searchTimer += g_gameTime->GetFrameDeltaTime();

		// 左右を見る
		m_searchMotionTimer += g_gameTime->GetFrameDeltaTime();

		float angle = sinf(m_searchMotionTimer * 2.0f) * 40.0f;

		Quaternion rot;
		rot.SetRotationDegY(m_rotationY + angle);
		m_modelRender.SetRotation(rot);

		if (m_searchTimer > 3.0f)
		{
			m_isSearching = false;
		}
		return;
	}

	if (m_isSearching)
	{
		return;
	}

	if (!m_hasDetectedPlayer)
	{
		return;
	}

	// プレイヤーを向く
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	toPlayer.y = 0.0f;

	if (toPlayer.LengthSq() > 0.0f)
	{
		toPlayer.Normalize();

		m_forward = toPlayer;

		m_rotation.SetRotationYFromDirectionXZ(m_forward);
		m_modelRender.SetRotation(m_rotation);

		m_rotationY = atan2f(m_forward.x, m_forward.z) * 180.0f / Math::PI;
	}
}

void FinalBoss::Shot()
{
	// プレイヤーを見つけていなければ撃たない
	if (!m_hasDetectedPlayer)
	{
		return;
	}

	if (m_isReposition)
	{
		return;
	}

	// クールタイム中
	if (m_shotCoolTime > 0.0f)
	{
		return;
	}

	PhaseChange();

	int randomNum = rand() % 2;
	AudioID id = (randomNum == 0) ? enSound_BossShotSE_01 : enSound_BossShotSE_02;

	m_audioManager->PlaySE(id, 1.0f, enSEPlay_AllowOverlap);

	m_shotCoolTime = m_shotCoolTimeReset;

	// 撃ったら移動開始
	m_isReposition = true;

	// 左右どちらかへ移動
	Vector3 right = Vector3(m_forward.z, 0.0f, -m_forward.x);

	float dir = (rand() % 2 == 0) ? 1.0f : -1.0f;
	float back = -400.0f;
	m_moveTarget = m_position + right * dir * 600.0f + m_forward * back;
}

void FinalBoss::CreateBullet(float angleOffset)
{
	m_bullet = NewGO<Bullet>(0, "bossBullet");

	Vector3 shotPos = m_position;

	Vector3 forward = Vector3::Front;
	m_rotation.Apply(forward);

	// Y軸回転を加える
	Quaternion rot;
	rot.SetRotationDegY(angleOffset);
	rot.Apply(forward);

	shotPos += forward * 450.0f;
	shotPos.y += 450.0f;

	m_bullet->SetPosition(shotPos);
	m_bullet->SetStartPosition(shotPos);
	m_bullet->SetMoveSpeed(forward * m_bulletSpeed);
}

void FinalBoss::PhaseChange()
{
	switch (m_game->GetDifficulty())
	{
	case EASY:
	{
		if (!m_firstPhaseChange)
		{
			// HP100～50%
			CreateBullet(0.0f);
		}
		else if (!m_secondPhaseChange)
		{
			// HP50～25%
			CreateBullet(-15.0f);
			CreateBullet(0.0f);
			CreateBullet(15.0f);
		}
		else
		{
			// HP25%以下
			CreateBullet(-30.0f);
			CreateBullet(-15.0f);
			CreateBullet(0.0f);
			CreateBullet(15.0f);
			CreateBullet(30.0f);
		}
		break;
	}

	case NORMAL:
	{
		if (!m_firstPhaseChange)
		{
			CreateBullet(0.0f);
		}
		else if (!m_secondPhaseChange)
		{
			CreateBullet(-20.0f);
			CreateBullet(0.0f);
			CreateBullet(20.0f);
		}
		else
		{
			CreateBullet(-40.0f);
			CreateBullet(-20.0f);
			CreateBullet(0.0f);
			CreateBullet(20.0f);
			CreateBullet(40.0f);
		}
		break;
	}

	case HARD:
	{
		if (!m_firstPhaseChange)
		{
			CreateBullet(-15.0f);
			CreateBullet(0.0f);
			CreateBullet(15.0f);
		}
		else if (!m_secondPhaseChange)
		{
			CreateBullet(-30.0f);
			CreateBullet(-15.0f);
			CreateBullet(0.0f);
			CreateBullet(15.0f);
			CreateBullet(30.0f);
		}
		else
		{
			CreateBullet(-45.0f);
			CreateBullet(-30.0f);
			CreateBullet(-15.0f);
			CreateBullet(0.0f);
			CreateBullet(15.0f);
			CreateBullet(30.0f);
			CreateBullet(45.0f);
		}
		break;
	}

	case LUNATIC:
	{
		if (!m_firstPhaseChange)
		{
			CreateBullet(-30.0f);
			CreateBullet(-15.0f);
			CreateBullet(0.0f);
			CreateBullet(15.0f);
			CreateBullet(30.0f);
		}
		else if (!m_secondPhaseChange)
		{
			CreateBullet(-45.0f);
			CreateBullet(-30.0f);
			CreateBullet(-15.0f);
			CreateBullet(0.0f);
			CreateBullet(15.0f);
			CreateBullet(30.0f);
			CreateBullet(45.0f);
		}
		else
		{
			CreateBullet(-60.0f);
			CreateBullet(-45.0f);
			CreateBullet(-30.0f);
			CreateBullet(-15.0f);
			CreateBullet(0.0f);
			CreateBullet(15.0f);
			CreateBullet(30.0f);
			CreateBullet(45.0f);
			CreateBullet(60.0f);
		}
		break;
	}
	}
}

void FinalBoss::Hit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("playerAttack");

	for (auto collision : collisions)
	{
		if (!collision->IsHit(m_characterController))
		{
			continue;
		}

		if (m_damageIntarvalTime > 0.0f)
		{
			continue;
		}

		int damage = m_player->GetAttackPower();

		int randomNum = rand() % 100 + 1;
		bool isCritical = (randomNum <= 5);

		if (!m_hasDetectedPlayer)
		{
			damage *= isCritical ? 3 : 1.5f;
			m_audioManager->PlaySE(enSound_BackstabSE, 1.0f, enSEPlay_AllowOverlap);
		}
		else
		{
			damage *= isCritical ? 1.5f : 1.0f;

			bool isHit = m_player->GetAttackHit();
			if (!isHit)
			{
				m_player->SetAttackHit(true);

				if (isCritical)
				{
					m_audioManager->PlaySE(enSound_CriticalSE, 1.0f, enSEPlay_AllowOverlap);
				}
				else
				{
					int r = rand() % 3;

					AudioID id;
					switch (r)
					{
					case 0: id = enSound_PlayerAttackSE_01; break;
					case 1: id = enSound_PlayerAttackSE_02; break;
					default: id = enSound_PlayerAttackSE_03; break;
					}

					m_audioManager->PlaySE(id, 1.0f, enSEPlay_AllowOverlap);
				}
			}
		}

		m_finalBossHp -= damage;

		if (m_finalBossHp < 0)
		{
			m_finalBossHp = 0;
		}

		m_damageIntarvalTime = 1.5f;

		//========================
		// ダメージ表示
		//========================
		DamageText* damageText = NewGO<DamageText>(0);

		Vector3 textPos = m_position;
		textPos.y += 250.0f;

		damageText->SetPosition(textPos);
		damageText->SetDamage(damage);

		// ★クリティカル演出
		if (isCritical)
		{
			damageText->SetCritical(true);   // 「CRITICAL!!」表示
		}
		if (isCritical)
		{
			StartHitStop(0.26f);
		}
		else
		{
			StartHitStop(0.2f);
		}
		if (isCritical)
		{
			GameCamera* camera = FindGO<GameCamera>("gameCamera");
			if (camera)
			{
				camera->StartShake(0.15f, 8.0f);
			}
		}
	}
}

const bool FinalBoss::SearchPlayer() const
{
	Vector3 diff = m_player->GetPosition() - m_position;

	if (diff.LengthSq() <= m_searchRange * m_searchRange)
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
		GameCamera* camera = FindGO<GameCamera>("gameCamera");
		if (camera)
		{
			camera->StartShake(0.3f, 20.0f);
		}
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

	EffectManager::GetInstance().PlayEffect(EffectManager::enEffect_BossExplosion,effectPos,50.0f);
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
	if (m_finalBossState == enFinalBossState_Shot)
	{
		return;
	}

	m_idleTimer = 0.0f;
	m_chaseTimer = 0.0f;

	Vector3 diff = m_player->GetPosition() - m_position;

	if (m_finalBossHp <= 0)
	{
		m_finalBossState = enFinalBossState_Death;
		return;
	}

	bool detected = SearchPlayer();

	if (detected)
	{
		m_lastPlayerPos = m_player->GetPosition();

		m_isSearching = false;
		m_searchTimer = 0.0f;
		// 初めて気付いた瞬間だけ
		if (!m_hasDetectedPlayer)
		{
			m_hasDetectedPlayer = true;

			m_isShowAlert = true;
			m_alertTime = 1.0f;
			m_alertScale = 0.0f;

			m_isShowQuestion = false;
			m_searchMotionTimer = 0.0f;
		}

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
		if (m_hasDetectedPlayer)
		{
			m_hasDetectedPlayer = false;

			m_isSearching = true;
			m_searchTimer = 0.0f;
			m_isShowQuestion = true;
			m_questionTime = 1.0f;
			m_questionScale = 0.0f;
			m_isLostWaiting = true;
			m_lostWaitTime = 2.0f;

			m_isShowAlert = false;
			m_searchMotionTimer = 0.0f;

			m_rotationY = atan2f(m_forward.x, m_forward.z) * 180.0f / Math::PI;
		}

		if ((m_startPosition - m_position).Length() > 100.0f)
		{
			m_finalBossState = enFinalBossState_Chase;
		}
		else
		{
			m_finalBossState = enFinalBossState_Idle;
		}
	}
}

void FinalBoss::IdleState()
{
	m_moveSpeed = Vector3::Zero;
}

void FinalBoss::WalkState()
{
	//float dist = (m_player->GetPosition() - m_position).Length();

	//if (dist <= 2000.0f)
	//{
	//	m_moveSpeed = Vector3::Zero;

	//	// クールタイムが終わっていれば撃つ
	//	if (m_shotCoolTime <= 0.0f)
	//	{
	//		m_finalBossState = enFinalBossState_Shot;
	//	}
	//	return;
	//}
}

void FinalBoss::ShotState()
{
	// アニメーション中は何もしない
	if (m_modelRender.IsPlayingAnimation())
	{
		return;
	}

	// アニメ終了
	m_isShot = false;

	float dist = (m_player->GetPosition() - m_position).Length();

	if (dist <= 2000.0f)
	{
		m_finalBossState = enFinalBossState_Idle;
	}
	else
	{
		m_finalBossState = enFinalBossState_Chase;
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
	if (m_viewHp > m_finalBossHp)
	{
		m_viewHp -= 200.0f * g_gameTime->GetFrameDeltaTime();

		if (m_viewHp < m_finalBossHp)
		{
			m_viewHp = (float)m_finalBossHp;
		}
	}

	float rate = m_viewHp / (float)m_finalBossMaxHp;

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

	if (m_isShowAlert)
	{
		m_alertMark.Draw(rc);
	}

	if (m_isShowQuestion)
	{
		m_questionMark.Draw(rc);
	}
}