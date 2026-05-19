#include "stdafx.h"
#include "MediumRobot.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "collision/CollisionObject.h"
#include "Game.h"
#include "DamageText.h"

MediumRobot::MediumRobot()
{
	
}

MediumRobot::~MediumRobot()
{
}

bool MediumRobot::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Enemy/mediumRobot/mediumRobotIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Enemy/mediumRobot/mediumRobotWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Death].Load("Assets/animData/Enemy/mediumRobot/mediumRobotDeath.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/Enemy/mediumRobot/mediumRobot.tkm", m_animationClips, enAnimationClip_Num);
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
	return true;
}

void MediumRobot::Update()
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
	if (!m_isDeath)
	{
		Move();

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

	MediumRobotHP();

	m_modelRender.Update();
}

void MediumRobot::Move()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 500 && m_timeCount == 0.0f && m_searchPlayer)
	{
		m_timeCount = 2.0f;
		Time();
	}
	if (m_searchPlayer)
	{
		toPlayer.Normalize();
		m_moveSpeed = toPlayer * 100.0f;
		m_moveSpeed.y = 0.0f;
	}
	else
	{
		m_moveSpeed = toPlayer * 0.0f;
	}

	m_position = m_characterController.Execute(m_moveSpeed, 2.0f / 60.0f);

	m_modelRender.SetPosition(m_position);
}

void MediumRobot::Rotation()
{
	if (m_searchPlayer)
	{
		Vector3 playerPos = m_player->GetPosition();
		Vector3 toPlayer = playerPos - m_position;
		float distToPlayer = toPlayer.Length();
		if (distToPlayer <= 1000)
		{
			toPlayer.Normalize();
			m_rotation.SetRotationYFromDirectionXZ(toPlayer);
		}
	}
	m_modelRender.SetRotation(m_rotation);
}

void MediumRobot::SearchPlayer()
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

void MediumRobot::Attack()
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

void MediumRobot::OnCollision()
{
	if(m_collisionObject != nullptr)
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
	m_collisionObject->SetName("mediumRobotAttack");
	
	m_attackCollisionLife = 0.1f; // 攻撃の当たり判定の寿命を設定
}

void MediumRobot::Time()
{
	m_timeCount -= g_gameTime->GetFrameDeltaTime();
	if (m_timeCount < 0.0f)
	{
		m_timeCount = 0.0f;
	}
}

void MediumRobot::Hit()
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

				m_mediumRobotHp -= damage;
				m_searchPlayer = true;
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
				m_mediumRobotHp -= damage;
				m_searchPlayer = true;
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

void MediumRobot::AttackHit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("mediumRobotAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_player->GetCharacterController()) == true)
		{
			int mediumRobotAttackPower = 0;
			mediumRobotAttackPower = GetAttackPower();
			m_player->TakeDamage(mediumRobotAttackPower, m_position);
		}
	}
}

void MediumRobot::DamageIntarval()
{
	m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_damageIntarvalTime < 0.0f)
	{
		m_damageIntarvalTime = 0.0f;
	}
}

void MediumRobot::Death()
{
	m_game->EnemyCount();
	MakeExplosionEffect();
	m_audioManager->PlaySE(enSound_EnemyDeathSE, 0.5f, enSEPlay_AllowOverlap);
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

void MediumRobot::MakeExplosionEffect()
{
	//爆発エフェクトを作る処理。
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(4);
	effectEmitter->SetScale(Vector3::One * 10.0f);
	Vector3 effectPos = m_position;
	effectEmitter->SetPosition(effectPos);
	effectEmitter->Play();
}

void MediumRobot::ManageState()
{
	switch (m_mediumRobotState)
	{
	case enMediumRobotState_Attack:
		AttackState();
		break;
	case enMediumRobotState_Idle:
		IdleState();
		break;
	case enMediumRobotState_Walk:
		WalkState();
		break;
	case enMediumRobotState_Death:
		DeathState();
		break;
	default:
		break;
	}
}

void MediumRobot::PlayAnimation()
{
	switch (m_mediumRobotState)
	{
	case enMediumRobotState_Attack:
		break;
	case enMediumRobotState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case enMediumRobotState_Walk:
		m_modelRender.PlayAnimation(enAnimationClip_Walk);
		break;
	case enMediumRobotState_Death:
		m_modelRender.PlayAnimation(enAnimationClip_Death);
		break;
	default:
		break;
	}
}

void MediumRobot::MediumRobotState()
{
	if (m_timeCount == 0 && !m_isDeath)
	{
		m_mediumRobotState = enMediumRobotState_Attack;
		m_isAttack = true;
	}
	if (m_mediumRobotHp <= 0)
	{
		m_mediumRobotState = enMediumRobotState_Death;
		m_isDeath = true;
	}
	else if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		m_mediumRobotState = enMediumRobotState_Walk;
	}
	else
	{
		m_mediumRobotState = enMediumRobotState_Idle;
	}
}

void MediumRobot::IdleState()
{
	MediumRobotState();
}

void MediumRobot::WalkState()
{
	MediumRobotState();
}

void MediumRobot::AttackState()
{
	MediumRobotState();
}

void MediumRobot::DeathState()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		Death();
	}
}

Vector3 MediumRobot::GetPosition()const
{
	return m_position;
}

void MediumRobot::MediumRobotHP()
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
	MaxHP = m_mediumRobotMaxHp;
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

void MediumRobot::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	if (m_isShowHP)
	{
		m_enemyHPFrame.Draw(rc);
		m_enemyHP.Draw(rc);
	}
}
