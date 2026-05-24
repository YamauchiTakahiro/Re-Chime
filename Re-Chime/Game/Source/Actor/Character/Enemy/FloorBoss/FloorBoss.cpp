#include "stdafx.h"
#include "FloorBoss.h"
#include "Source/Actor/Character/Player/Player.h"
#include "collision/CollisionObject.h"
#include "Game.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"	
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "DamageText.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"

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
	m_characterController.Init(900.0f, 300.0f, m_position);
	m_enemyHP.Init("Assets/UIData/HP.DDs", 1024.0f, 128.0f);
	m_enemyHP.SetPivot(Vector2(0.0f, 0.5f));
	m_enemyHP.Update();
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
		case Game::EASY:
			m_floorBossHP = 150;
			m_floorBossMaxHP = 150;
			m_attackPower = 10;
			break;

		case Game::NORMAL:
			m_floorBossHP = 250;
			m_floorBossMaxHP = 250;
			m_attackPower = 20;
			break;

		case Game::HARD:
			m_floorBossHP = 400;
			m_floorBossMaxHP = 400;
			m_attackPower = 35;
			break;

		case Game::LUNATIC:
			m_floorBossHP = 700;
			m_floorBossMaxHP = 700;
			m_attackPower = 50;
			break;
		}
	}

	return true;
}

void FloorBoss::Update()
{
	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);

	if (isPause || m_game->IsGameStop())
	{
		return;
	}

	Move();

	Rotation();

	Attack();

	Time();

	Hit();

	DamageIntarval();

	AttackHit();

	ManageState();
	//EnemyHP();

	ManageState();

	PlayAnimation();
	m_modelRender.Update();
}

void FloorBoss::Move()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 600 && m_timeCount == 0.0f)
	{
		m_timeCount = 2.0f;
		Time();
	}
	if (distToPlayer <= 1200)
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
		m_moveSpeed.y -= 40.0f;
	}

	m_position = m_characterController.Execute(m_moveSpeed, 2.0f / 60.0f);

	m_modelRender.SetPosition(m_position);
}

void FloorBoss::Rotation()
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
	m_collisionObject = NewGO<CollisionObject>(0);

	Vector3 collisionPos = m_position;
	m_forward = Vector3::Front;
	m_rotation.Apply(m_forward);
	collisionPos += m_forward * 250.0f;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("floorBossAttack");

	m_attackCollisionLife = 0.1f; // 攻撃の当たり判定の寿命を設定
}

void FloorBoss::Time()
{
	m_timeCount -= g_gameTime->GetFrameDeltaTime();
	if (m_timeCount < 0.0f)
	{
		m_timeCount = 0.0f;
	}
}

void FloorBoss::Hit()
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
				m_floorBossHP -= damage;
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
				m_floorBossHP -= damage;
			}
			m_damageIntarvalTime = 1.5f;
			m_player->SetAttackHit(true);

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

void FloorBoss::AttackHit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("floorBossAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_player->GetCharacterController()) == true)
		{
			int floorBossAttackPower = 0;
			floorBossAttackPower = GetAttackPower();
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
		m_game->EnemyCount();
		m_audioManager->PlaySE(enSound_EnemyDeathSE, 0.5f, enSEPlay_AllowOverlap);
		MakeExplosionEffect();
		int randomNum = rand() % 100 + 1;
		if (randomNum <= 20)
		{
			m_attackSpeedBuff = NewGO<AttackSpeedBuff>(0);
			m_attackSpeedBuff->SetPosition(m_position);
			m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
		}
		else if (randomNum > 20 && randomNum <= 40)
		{
			m_powerBuff = NewGO<PowerBuff>(0);
			m_powerBuff->SetPosition(m_position);
			m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
		}
		else if (randomNum > 40 && randomNum <= 60)
		{
			m_heal = NewGO<Heal>(0);
			m_heal->SetPosition(m_position);
			m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
		}
		DeleteGO(this);
	}
}

void FloorBoss::MakeExplosionEffect()
{
	// 爆発エフェクトの生成処理をここに実装
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(5);
	effectEmitter->SetScale(Vector3::One * 10.0f);
	Vector3 effectPos = m_position;
	effectEmitter->SetPosition(effectPos);
	effectEmitter->Play();
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
	case enFloorBossState_Death:
		DeathState();
		break;
	default:
		break;
	}
}

void FloorBoss::FloorBossHP()
{

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
	case enFloorBossState_Death:
		m_modelRender.PlayAnimation(enAnimationClip_Death);
		break;
	default:
		break;
	}
}

void FloorBoss::FloorBossState()
{
	if (m_floorBossHP <= 0)
	{
		m_floorBossState = enFloorBossState_Death;
	}
	else if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
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

void FloorBoss::DeathState()
{
	if(m_modelRender.IsPlayingAnimation() == false)
	{
		Death();
	}
}

void FloorBoss::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	if (m_isShowHP)
	{
		m_enemyHP.Draw(rc);
	}
}