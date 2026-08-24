#include "stdafx.h"
#include "FloorBoss.h"
#include "Source/Actor/Character/Player/Player.h"
#include "collision/CollisionObject.h"
#include "Game.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"	
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "Source/Manager/AudioManager/AudioManager.h"
#include "Source/UIBase/DamageText/DamageText.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/Manager/EffectManager/EffectManager.h"
#include "Source/Camera/GameCamera.h"

FloorBoss::FloorBoss()
{
	
}

FloorBoss::~FloorBoss()
{
}

bool FloorBoss::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Enemy/floorBoss/floorBossIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Enemy/floorBoss/floorBossWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Death].Load("Assets/animData/Enemy/floorBoss/floorBossDeath.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/Enemy/floorBoss/FloorBoss.tkm", m_animationClips, enAnimationClip_Num, enModelUpAxisY);
	m_characterController.Init(300.0f, 200.0f, m_position);

	m_bossHPFrame.Init("Assets/UIData/enemyHPFrame.DDs", 1024, 128.0f);
	m_bossHPFrame.SetPosition(Vector3(10.5f, 450.0f, 0.0f));
	m_bossHPFrame.SetScale(Vector3(1.52f, 0.3f, 1.0f));
	m_bossHPFrame.Update();

	m_bossHPBar.Init("Assets/UIData/enemyHPBar.DDs", 1024, 128.0f);
	m_bossHPBar.SetPosition(Vector3(-760.0f, 447.0f, 0.0f));
	m_bossHPBar.SetScale(Vector3(1.48f, 0.3f, 1.0f));
	m_bossHPBar.SetPivot(Vector2(0.0f, 0.5f));
	m_bossHPBar.Update();

	m_alertMark.Init("Assets/UIData/AlertMark.DDs", 128, 128);
	m_alertMark.SetScale(Vector3(0.7f, 0.7f, 1));
	m_alertMark.Update();

	m_questionMark.Init("Assets/UIData/QuestionMark.DDS", 128, 128);
	m_questionMark.SetScale(Vector3(0.7f, 0.7f, 1));
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
			m_floorBossHP = 150;
			m_floorBossMaxHP = 150;
			m_attackPower = 10;
			m_attackIntervalTime = 3.0f;
			m_moveSpeedValue = 180.0f;
			break;

		case NORMAL:
			m_floorBossHP = 250;
			m_floorBossMaxHP = 250;
			m_attackPower = 20;
			m_attackIntervalTime = 3.0f;
			m_moveSpeedValue = 230.0f;
			break;

		case HARD:
			m_floorBossHP = 400;
			m_floorBossMaxHP = 400;
			m_attackPower = 35;
			m_attackIntervalTime = 2.5f;
			m_moveSpeedValue = 250.0f;
			break;

		case LUNATIC:
			m_floorBossHP = 700;
			m_floorBossMaxHP = 700;
			m_attackPower = 50;
			m_attackIntervalTime = 2.0f;
			m_moveSpeedValue = 400.0f;
			break;
		}
	}
	m_viewHp = (float)m_floorBossHP;

	return true;
}

void FloorBoss::Update()
{
	UpdateHitStop();

	if (IsHitStop())
	{
		m_modelRender.Update();
		FloorBossHP();
		return;
	}

	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);

	if (isPause || m_game->IsGameStop())
	{
		return;
	}

	float dist = (m_player->GetPosition() - m_position).Length();

	bool detected = (dist <= 1500.0f);

	if (detected && !m_hasDetectedPlayer)
	{
		m_hasDetectedPlayer = true;

		m_isShowAlert = true;
		m_alertTime = 1.0f;
		m_alertScale = 0.0f;

		m_isShowQuestion = false;
	}

	if (!detected && m_hasDetectedPlayer)
	{
		m_hasDetectedPlayer = false;

		m_isShowQuestion = true;
		m_questionTime = 1.0f;
		m_questionScale = 0.0f;

		m_isShowAlert = false;
	}

	Move();

	Rotation();

	Time();

	Hit();

	BodyPush();

	DamageIntarval();

	AttackHit();

	FloorBossHP();

	ManageState();

	PlayAnimation();

	m_attackCollisionLife -= g_gameTime->GetFrameDeltaTime();

	if (m_attackCollisionLife <= 0.0f)
	{
		DeleteGO(m_collisionObject);
		m_collisionObject = nullptr;
	}

	if (m_isShowAlert)
	{
		m_alertTime -= g_gameTime->GetFrameDeltaTime();

		if (m_alertTime <= 0.0f)
		{
			m_isShowAlert = false;
		}

		Vector3 pos = m_position;
		pos.y += 700.0f;

		Vector2 screenPos;
		g_camera3D->CalcScreenPositionFromWorldPosition(screenPos, pos);

		m_alertMark.SetPosition(Vector3(screenPos.x, screenPos.y, 0));

		m_alertScale += 5.0f * g_gameTime->GetFrameDeltaTime();

		if (m_alertScale > 0.3f)
		{
			m_alertScale = 0.3f;
		}

		m_alertMark.SetScale(Vector3(m_alertScale, m_alertScale, 1));
		m_alertMark.Update();
	}

	if (m_isShowQuestion)
	{
		m_questionTime -= g_gameTime->GetFrameDeltaTime();

		if (m_questionTime <= 0.0f)
		{
			m_isShowQuestion = false;
		}

		Vector3 pos = m_position;
		pos.y += 700.0f;

		Vector2 screenPos;
		g_camera3D->CalcScreenPositionFromWorldPosition(screenPos, pos);

		m_questionMark.SetPosition(Vector3(screenPos.x, screenPos.y, 0));

		m_questionScale += 5.0f * g_gameTime->GetFrameDeltaTime();

		if (m_questionScale > 0.3f)
		{
			m_questionScale = 0.3f;
		}

		m_questionMark.SetScale(Vector3(m_questionScale, m_questionScale, 1));
		m_questionMark.Update();
	}

	m_modelRender.Update();
}

void FloorBoss::Move()
{
	if (m_tackleEndStunTime > 0.0f)
	{
		m_moveSpeed = Vector3::Zero;
		return;
	}

	if (m_floorBossHP <= 0 || m_floorBossState == enFloorBossState_Death)
	{
		m_moveSpeed = Vector3::Zero;
		return;
	}

	if (m_floorBossHP == enFloorBossState_Attack)
	{
		return;
	}

	if (m_floorBossState == enFloorBossState_Hit)
	{
		m_moveSpeed = Vector3::Zero;

		m_position = m_characterController.Execute(m_moveSpeed, 2.0f / 60.0f);

		m_modelRender.SetPosition(m_position);
		return;
	}

	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();

	const float attackDistance = 500.0f;

	if (distToPlayer > attackDistance && distToPlayer <= 1500.0f)
	{
		toPlayer.Normalize();
		m_moveSpeed = toPlayer * m_moveSpeedValue;
		m_moveSpeed.y = 0.0f;
	}
	else
	{
		m_moveSpeed = Vector3::Zero;
	}

	if (m_characterController.IsOnGround() == false)
	{
		//m_moveSpeed.y -= 40.0f;
	}

	m_position = m_characterController.Execute(m_moveSpeed, 2.0f / 60.0f);

	m_modelRender.SetPosition(m_position);
}

void FloorBoss::Rotation()
{
	if (m_floorBossHP <= 0 || m_floorBossState == enFloorBossState_Death)
	{
		return;
	}

	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 1500)
	{
		toPlayer.Normalize();
		m_rotation.SetRotationYFromDirectionXZ(toPlayer);
	}
	m_modelRender.SetRotation(m_rotation);
}

void FloorBoss::Attack()
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
}

void FloorBoss::OnCollision()
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

	collisionPos += m_forward * 450.0f;

	m_collisionObject->CreateSphere(
		collisionPos,
		Quaternion::Identity,
		350.0f
	);

	m_collisionObject->SetName("floorBossAttack");

	m_attackCollisionLife = 0.1f;
}

void FloorBoss::Time()
{
	m_timeCount -= g_gameTime->GetFrameDeltaTime();
	if (m_timeCount < 0.0f)
	{
		m_timeCount = 0.0f;
	}

	m_bodyPushCoolTime -= g_gameTime->GetFrameDeltaTime();

	if (m_bodyPushCoolTime < 0.0f)
	{
		m_bodyPushCoolTime = 0.0f;
	}

	m_tackleEndStunTime -= g_gameTime->GetFrameDeltaTime();

	if (m_tackleEndStunTime < 0.0f)
	{
		m_tackleEndStunTime = 0.0f;
	}
}

void FloorBoss::Hit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("playerAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_characterController) == true && m_damageIntarvalTime == 0.0f)
		{
			int damage = m_player->GetAttackPower();

			bool isCritical = (rand() % 100) < 5;

			if (isCritical)
			{
				damage *= 2;

				bool isHit = m_player->GetAttackHit();
				if (!isHit)
				{
					m_player->SetAttackHit(true);
					m_audioManager->PlaySE(enSound_CriticalSE, 1.0f, enSEPlay_AllowOverlap);
				}
			}
			else
			{
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

					m_audioManager->PlaySE(id, 1.0f, enSEPlay_AllowOverlap);
				}
			}

			m_floorBossHP -= damage;
			if (m_floorBossHP < 0)
			{
				m_floorBossHP = 0;
			}
			m_damageIntarvalTime = 1.5f;

			if (m_floorBossHP > 0)
			{
				m_floorBossState = enFloorBossState_Hit;
				m_hitStateTimer = 0.5f;
			}
			m_player->SetAttackHit(true);

			//========================
            // ダメージ表示生成
            //========================
			DamageText* damageText = NewGO<DamageText>(0);

			Vector3 textPos = m_position;

			textPos.y += 250.0f;

			damageText->SetPosition(textPos);
			damageText->SetDamage(damage);
			damageText->SetCritical(isCritical);
			if (isCritical)
			{
				StartHitStop(0.21f);

				GameCamera* camera = FindGO<GameCamera>("gameCamera");
				if (camera)
				{
					camera->StartShake(0.15f, 8.0f);
				}
			}
			else
			{
				StartHitStop(0.15f);
			}
		}
	}
}

void FloorBoss::AttackHit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("floorBossAttack");

	for (auto collision : collisions)
	{
		if (collision->IsHit(m_player->GetCharacterController()) == true)
		{
			int floorBossAttackPower = GetAttackPower();

			m_player->TakeDamage(floorBossAttackPower, m_position);
		}
	}
}

void FloorBoss::DamageIntarval()
{
	m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_damageIntarvalTime < 0.0f)
	{
		m_damageIntarvalTime = 0.0f;
	}
}

void FloorBoss::Death()
{
	if (m_floorBossHP <= 0)
	{
		//========================================
		// 死亡演出
		//========================================

		// 強めのカメラシェイク
		GameCamera* camera = FindGO<GameCamera>("gameCamera");
		if (camera)
		{
			camera->StartShake(0.8f, 30.0f);
		}

		// 死亡SE
		m_audioManager->PlaySE(
			enSound_EnemyDeathSE,
			0.7f,
			enSEPlay_AllowOverlap
		);

		// 爆発エフェクト
		MakeExplosionEffect();

		// 敵撃破数
		m_game->EnemyCount();

		// アイテムドロップ
		int randomNum = rand() % 100 + 1;

		if (randomNum <= 20)
		{
			m_attackSpeedBuff = NewGO<AttackSpeedBuff>(0);
			m_attackSpeedBuff->SetPosition(m_position);
			m_audioManager->PlaySE(
				enSound_ItemDropSE,
				0.5f,
				enSEPlay_AllowOverlap
			);
		}
		else if (randomNum <= 40)
		{
			m_powerBuff = NewGO<PowerBuff>(0);
			m_powerBuff->SetPosition(m_position);
			m_audioManager->PlaySE(
				enSound_ItemDropSE,
				0.5f,
				enSEPlay_AllowOverlap
			);
		}
		else if (randomNum <= 60)
		{
			m_heal = NewGO<Heal>(0);
			m_heal->SetPosition(m_position);
			m_audioManager->PlaySE(
				enSound_ItemDropSE,
				0.5f,
				enSEPlay_AllowOverlap
			);
		}

		DeleteGO(this);
	}
}

void FloorBoss::MakeExplosionEffect()
{
	//爆発エフェクトの生成
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;

	EffectManager::GetInstance().PlayEffect(EffectManager::enEffect_BossExplosion,effectPos,50.0f);
}

void FloorBoss::MakeNoticeCircleEffect()
{
	//予告円エフェクトの生成
	Vector3 effectPos = m_position;
	m_forward = Vector3::Front;
	m_rotation.Apply(m_forward);
	effectPos += m_forward * 450.0f;
	EffectManager::GetInstance().PlayEffect(EffectManager::enEffect_NoticeCircle,effectPos,200.0f);
}

void FloorBoss::ManageState()
{
	switch (m_floorBossState)
	{
	case enFloorBossState_Idle:
		IdleState();
		break;

	case enFloorBossState_Walk:
		WalkState();
		break;

	case enFloorBossState_Attack:
		AttackState();
		break;

	case enFloorBossState_Hit:
		HitState();
		break;

	case enFloorBossState_Death:
		DeathState();
		break;

	default:
		break;
	}
}

void FloorBoss::FloorBossHP()
{
	if (m_viewHp > m_floorBossHP)
	{
		m_viewHp -= 200.0f * g_gameTime->GetFrameDeltaTime();

		if (m_viewHp < m_floorBossHP)
		{
			m_viewHp = (float)m_floorBossHP;
		}
	}

	float rate = m_viewHp / (float)m_floorBossMaxHP;

	Vector3 scale = { 1.5f, 0.3f, 1.0f };
	scale.x *= rate;

	m_bossHPBar.SetScale(scale);

	if (m_floorBossHP <= m_floorBossMaxHP / 4)
	{
		m_bossHPBar.SetMulColor(g_vec4Red);
	}
	else
	{
		m_bossHPBar.SetMulColor(g_vec4Green);
	}

	m_bossHPBar.Update();

	float dist = (m_player->GetPosition() - m_position).Length();

	m_isShowHP = (dist <= 2000.0f);
}

void FloorBoss::PlayAnimation()
{
	switch (m_floorBossState)
	{
	case enFloorBossState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;

	case enFloorBossState_Walk:
		m_modelRender.PlayAnimation(enAnimationClip_Walk);
		break;

	case enFloorBossState_Attack:

		// 後退中・突進中は歩きアニメーション
		if (m_attackPhase == enAttackPhase_BackStep ||
			m_attackPhase == enAttackPhase_Dash)
		{
			m_modelRender.PlayAnimation(enAnimationClip_Walk);
		}

		break;

	case enFloorBossState_Hit:
		break;

	case enFloorBossState_Death:
		m_modelRender.PlayAnimation(enAnimationClip_Death);
		break;

	default:
		break;
	}
}

void FloorBoss::FloorBossState()
{
	// タックル終了後の硬直中
	if (m_tackleEndStunTime > 0.0f)
	{
		m_floorBossState = enFloorBossState_Idle;
		return;
	}

	const float attackDistance = 500.0f;

	Vector3 playerPos = m_player->GetPosition();
	float dist = (playerPos - m_position).Length();

	if (m_floorBossHP <= 0)
	{
		m_floorBossHP = 0;
		m_floorBossState = enFloorBossState_Death;
		m_moveSpeed = Vector3::Zero;
	}
	else if (dist <= attackDistance &&
		m_timeCount <= 0.0f &&
		m_floorBossState != enFloorBossState_Attack)
	{
		m_floorBossState = enFloorBossState_Attack;

		m_attackStateTimer = 0.8f;
		m_attackPhase = enAttackPhase_Warn;

		m_timeCount = m_attackIntervalTime;
		MakeNoticeCircleEffect();
	}
	else if (fabsf(m_moveSpeed.x) > 0.01f ||
		fabsf(m_moveSpeed.z) > 0.01f)
	{
		m_floorBossState = enFloorBossState_Walk;
	}
	else
	{
		m_floorBossState = enFloorBossState_Idle;
	}
}
void FloorBoss::IdleState()
{
	FloorBossState();
}

void FloorBoss::WalkState()
{
	FloorBossState();
}

void FloorBoss::AttackState()
{
	float dt = g_gameTime->GetFrameDeltaTime();

	m_moveSpeed = Vector3::Zero;

	//========================================
	// ① 予備動作
	//========================================	
	if (m_attackPhase == enAttackPhase_Warn)
	{
		m_attackStateTimer -= dt;

		if (m_attackStateTimer <= 0.0f)
		{
			GameCamera* camera = FindGO<GameCamera>("gameCamera");
			if (camera)
			{
				camera->StartShake(0.15f, 10.0f);
			}
			//プレイヤーの方向を記録
			m_dashDirection = m_player->GetPosition() - m_position;
			m_dashDirection.y = 0.0f;

			if (m_dashDirection.Length() > 0.01f)
			{
				m_dashDirection.Normalize();
			}

			m_backStepDirection.x = -m_dashDirection.x;
			m_backStepDirection.y = 0.0f;
			m_backStepDirection.z = -m_dashDirection.z;

			m_backStepTimer = 0.5f;

			m_backStepSpeed = 1100.0f;

			m_attackPhase = enAttackPhase_BackStep;
		}
		return;
	}

	//========================================
    // ② 後ろに下がってタックルの溜め
    //========================================
	if (m_attackPhase == enAttackPhase_BackStep)
	{
		m_backStepTimer -= dt;

		m_moveSpeed.x = m_backStepDirection.x * m_backStepSpeed;
		m_moveSpeed.y = 0.0f;
		m_moveSpeed.z = m_backStepDirection.z * m_backStepSpeed;

		m_position = m_characterController.Execute(
			m_moveSpeed,
			dt
		);

		m_modelRender.SetPosition(m_position);

		// プレイヤーの方向を向き続ける
		if (m_dashDirection.Length() > 0.01f)
		{
			m_rotation.SetRotationYFromDirectionXZ(m_dashDirection);
			m_modelRender.SetRotation(m_rotation);
		}

		if (m_backStepTimer <= 0.0f)
		{
			m_backStepTimer = 0.0f;

			m_moveSpeed = Vector3::Zero;

			m_dashAttackTimer = 0.45f;

			// タックル開始前の強い振動
			GameCamera* camera = FindGO<GameCamera>("gameCamera");
			if (camera)
			{
				camera->StartShake(0.15f, 10.0f);
			}

			m_attackPhase = enAttackPhase_Dash;
			m_dashHitFlag = false;
		}

		return;
	}

	//========================================
    // ③ 突進
    //========================================
	if (m_attackPhase == enAttackPhase_Dash)
	{
		m_dashAttackTimer -= dt;

		m_moveSpeed = m_dashDirection * 2200.0f;
		m_moveSpeed.y = 0.0f;

		// 突進移動
		m_position = m_characterController.Execute(m_moveSpeed, dt);

		m_modelRender.SetPosition(m_position);

		if (m_dashDirection.Length() > 0.01f)
		{
			m_rotation.SetRotationYFromDirectionXZ(m_dashDirection);
			m_modelRender.SetRotation(m_rotation);
		}

		// 突進中の攻撃判定
		OnCollision();

		GameCamera* camera = FindGO<GameCamera>("gameCamera");
		if (camera)
		{
			camera->StartShake(0.02f, 2.0f);
		}

		if (m_dashAttackTimer <= 0.0f)
		{
			m_dashAttackTimer = 0.0f;

			m_moveSpeed = Vector3::Zero;

			GameCamera* camera = FindGO<GameCamera>("gameCamera");
			if (camera)
			{
				camera->StartShake(0.3f, 24.0f);
			}

			m_attackStateTimer = 0.15f;

			m_attackPhase = enAttackPhase_Active;
		}

		return;
	}

	//========================================
    // ④ 攻撃中
    //========================================
	if (m_attackPhase == enAttackPhase_Active)
	{
		m_attackStateTimer -= dt;

		m_moveSpeed = Vector3::Zero;

		if (m_attackStateTimer <= 0.0f)
		{
			m_attackStateTimer = 0.0f;

			// タックル終了後の硬直時間
			m_attackAfterDelay = 1.5f;

			m_attackPhase = enAttackPhase_End;
		}

		return;
	}

	//========================================
    // ⑤ 攻撃終了
    //========================================
	if (m_attackPhase == enAttackPhase_End)
	{
		m_attackAfterDelay -= dt;

		// 硬直中は完全停止
		m_moveSpeed = Vector3::Zero;

		if (m_attackAfterDelay <= 0.0f)
		{
			m_attackAfterDelay = 0.0f;

			m_attackPhase = enAttackPhase_Warn;
			m_floorBossState = enFloorBossState_Idle;
		}

		return;
	}
}

void FloorBoss::DeathState()
{
	// 死亡アニメーションがまだ終わっていない
	if (m_modelRender.IsPlayingAnimation())
	{
		return;
	}

	// 死亡演出をまだ開始していない
	if (!m_isDeathEffectStarted)
	{
		m_isDeathEffectStarted = true;

		GameCamera* camera = FindGO<GameCamera>("gameCamera");
		if (camera)
		{
			// 爆発直前の強い揺れ
			camera->StartShake(0.8f, 30.0f);
		}

		// 爆発SE
		m_audioManager->PlaySE(enSound_EnemyDeathSE, 0.7f, enSEPlay_AllowOverlap);

		// 爆発エフェクト
		MakeExplosionEffect();

		// 撃破数
		m_game->EnemyCount();

		// アイテムドロップ
		int randomNum = rand() % 100 + 1;

		if (randomNum <= 20)
		{
			m_attackSpeedBuff = NewGO<AttackSpeedBuff>(0);
			m_attackSpeedBuff->SetPosition(m_position);

			m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
		}
		else if (randomNum <= 40)
		{
			m_powerBuff = NewGO<PowerBuff>(0);
			m_powerBuff->SetPosition(m_position);

			m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
		}
		else if (randomNum <= 60)
		{
			m_heal = NewGO<Heal>(0);
			m_heal->SetPosition(m_position);

			m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
		}

		// 演出開始
		m_deathEffectTimer = 0.5f;

		return;
	}

	// 爆発後、少し待ってから消す
	m_deathEffectTimer -= g_gameTime->GetFrameDeltaTime();

	if (m_deathEffectTimer <= 0.0f)
	{
		DeleteGO(this);
	}
}

void FloorBoss::BodyPush()
{
	if (m_floorBossHP <= 0 || m_floorBossState == enFloorBossState_Death)
	{
		return;
	}

	if (m_floorBossState == enFloorBossState_Attack)
	{
		return;
	}

	if (m_bodyPushCoolTime > 0.0f)
	{
		return;
	}

	Vector3 dir = m_player->GetPosition() - m_position;
	dir.y = 0.0f;

	float distance = dir.Length();

	if (distance >= 500.0f)
	{
		return;
	}

	if (distance < 0.01f)
	{
		return;
	}

	dir.Normalize();

	// FloorBossの正面
	Vector3 forward = Vector3::Front;
	m_rotation.Apply(forward);
	forward.y = 0.0f;

	if (forward.LengthSq() > 0.01f)
	{
		forward.Normalize();
	}

	// 正面にいる場合はノックバックさせない
	float dot = forward.x * dir.x + forward.z * dir.z;

	if (dot > 0.3f)
	{
		return;
	}

	// 敵から離れる
	m_player->KnockBackOnly(m_position, 1500.0f);

	m_bodyPushCoolTime = 0.3f;
}

void FloorBoss::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	if (m_isShowHP)
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

void FloorBoss::HitState()
{
	m_moveSpeed = Vector3::Zero;

	m_hitStateTimer -= g_gameTime->GetFrameDeltaTime();

	if (m_hitStateTimer <= 0.0f)
	{
		m_hitStateTimer = 0.0f;

		Vector3 toPlayer = m_player->GetPosition() - m_position;
		float distToPlayer = toPlayer.Length();

		// 攻撃範囲より遠いなら再びプレイヤーへ向かう
		if (distToPlayer > 500.0f && distToPlayer <= 1500.0f)
		{
			m_floorBossState = enFloorBossState_Walk;
		}
		else
		{
			m_floorBossState = enFloorBossState_Idle;
		}
	}
}