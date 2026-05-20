#include "stdafx.h"
#include "SmallRobot.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "collision/CollisionObject.h"
#include "Game.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "DamageText.h"

SmallRobot::SmallRobot()
{
	
}

SmallRobot::~SmallRobot()
{

}

bool SmallRobot::Start()
{
	/*m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Enemy/smallRobot/smallRobotIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Enemy/smallRobot/smallRobotWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);*/
	m_modelRender.Init("Assets/modelData/Enemy/smallRobot/smallRobot.tkm"/*, m_animationClips, enAnimationClip_Num*/);

	m_enemyHP.Init("Assets/UIData/enemyHPBar.DDs", 1024.0f, 128.0f);
	//m_enemyHP.SetScale(Vector3(0.41f, 3.0f, 0.5f));
	m_enemyHP.SetPivot(Vector2(0.0f, 0.5f));
	m_enemyHP.Update();

	m_enemyHPFrame.Init("Assets/UIData/enemyHPFrame.DDs", 1024, 128.0f);
	m_enemyHPFrame.SetScale(Vector3(0.29f, 0.29f, 0.0f));
	m_enemyHPFrame.SetPivot(Vector2(0.02f, 0.4f));
	m_enemyHPFrame.Update();

	m_characterController.Init(200.0f, 100.0f, m_position);
	m_player = FindGO<Player>("player");
	m_game = FindGO<Game>("game");

	m_audioManager = FindGO<AudioManager>("audioManager");

	return true;
}

void SmallRobot::Update()
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
		Move();

		Rotation();
	}

	Attack();

	SearchPlayer();

	Time();

	Hit();

	DamageIntarval();

	AttackHit();

	EnemyHP();

	ManageState();

	//PlayAnimation();
	m_modelRender.Update();
}

void SmallRobot::Move()
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

void SmallRobot::Rotation()
{
	if(m_searchPlayer)
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

void SmallRobot::SearchPlayer()
{
	m_searchPlayer = false;

	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);

	Vector3 playerPos = m_player->GetPosition();
	Vector3 diff = playerPos - m_position;

	if(diff.Length() <= 2000.0f)
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

void SmallRobot::Attack()
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

void SmallRobot::OnCollision()
{
	m_collisionObject = NewGO<CollisionObject>(0);

	Vector3 collisionPos = m_position;
	m_forward = Vector3::Front;
	m_rotation.Apply(m_forward);
	collisionPos += m_forward * 250.0f;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("smallRobotAttack");

	m_attackCollisionLife = 0.1f;
}

void SmallRobot::Time()
{
	m_timeCount -= g_gameTime->GetFrameDeltaTime();
	if (m_timeCount < 0.0f)
	{
		m_timeCount = 0.0f;
	}
}

void SmallRobot::Hit()
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
					damage *= 4;
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
				}
				
				m_smallRobotHp -= damage;
				m_searchPlayer = true;
			}
			else
			{
				if (!m_searchPlayer)
				{
					damage *= 2;
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
				m_smallRobotHp -= damage;
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
		}
	}
}

void SmallRobot::AttackHit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("smallRobotAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_player->GetCharacterController()) == true)
		{
			int smallRobotAttackPower = 0;
			smallRobotAttackPower = GetAttackPower();
			m_player->TakeDamage(smallRobotAttackPower, m_position);
		}
	}
}

void SmallRobot::DamageIntarval()
{
	m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_damageIntarvalTime < 0.0f)
	{
		m_damageIntarvalTime = 0.0f;
	}
}

void SmallRobot::Death()
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

void SmallRobot::EnemyHP()
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
	MaxHP = m_smallRobotMaxHp;
	float Wari = (float)nowHP / (float)MaxHP;
	Vector3 scale = { 0.28f, 0.28f, 0.5f };
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

void SmallRobot::MakeExplosionEffect()
{
	//爆発エフェクトの生成
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(4);
	effectEmitter->SetScale(Vector3::One * 10.0f);
	Vector3 effectPos = m_position;
	effectEmitter->SetPosition(effectPos);
	effectEmitter->Play();
}

void SmallRobot::EnemyState()
{
	if (m_timeCount == 0 && !m_isDeath)
	{
		m_smallRobotState = enSmallRobotState_Attack;
		m_isAttack = true;
	}
	if (m_smallRobotHp <= 0)
	{
		m_smallRobotState = enSmallRobotState_Death;
		m_isDeath = true;
	}
	else if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		m_smallRobotState = enSmallRobotState_Walk;
	}
	else
	{
		m_smallRobotState = enSmallRobotState_Idle;
	}
}

void SmallRobot::IdleState()
{
	EnemyState();
}

void SmallRobot::WalkState()
{
	EnemyState();
}

void SmallRobot::AttackState()
{
	EnemyState();
}

void SmallRobot::DeathState()
{
	Death();
}

void SmallRobot::ManageState()
{
	switch (m_smallRobotState)
	{
	case enSmallRobotState_Idle:
		IdleState();
		break;
	case enSmallRobotState_Walk:
		WalkState();
		break;
	case enSmallRobotState_Attack:
		AttackState();
		break;
	case enSmallRobotState_Death:
		DeathState();
		break;
	case enSmallRobotState_Num:
		break;
	default:
		break;
	}
}
//void SmallRobot::PlayAnimation()
//{
//	switch (m_smallRobotState)
//	{
//	case enSmallRobotState_Idle:
//		m_modelRender.PlayAnimation(enAnimationClip_Idle);
//		break;
//	case enSmallRobotState_Walk:
//		m_modelRender.PlayAnimation(enAnimationClip_Walk);
//		break;
//	case enSmallRobotState_Num:
//		break;
//	default:
//		break;
//	}
//}

void SmallRobot::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	
	if (m_isShowHP)
	{
		m_enemyHPFrame.Draw(rc);
		m_enemyHP.Draw(rc);
	}
}