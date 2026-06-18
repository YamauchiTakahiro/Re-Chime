#include "stdafx.h"
#include "SmallRobot.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "collision/CollisionObject.h"
#include "Game.h"
#include "Source/Manager/AudioManager/AudioManager.h"
#include "Source/UIBase/DamageText/DamageText.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/Manager/EffectManager/EffectManager.h"
#include "Source/Manager/AudioManager/AudioManager.h"

SmallRobot::SmallRobot()
{
	
}

SmallRobot::~SmallRobot()
{
	DeleteGO(m_collisionObject);
}

bool SmallRobot::Start()
{
	m_modelRender.Init("Assets/modelData/Enemy/smallRobot/smallRobot.tkm");

	m_enemyHP.Init("Assets/UIData/enemyHPBar.DDs", 1024.0f, 128.0f);

	m_enemyHP.SetPivot(Vector2(0.0f, 0.5f));
	m_enemyHP.Update();

	m_enemyHPFrame.Init("Assets/UIData/enemyHPFrame.DDs", 1024, 128.0f);
	m_enemyHPFrame.SetScale(Vector3(0.29f, 0.29f, 0.0f));
	m_enemyHPFrame.SetPivot(Vector2(0.02f, 0.4f));
	m_enemyHPFrame.Update();

	m_characterController.Init(200.0f, 100.0f, m_position);

	m_player = FindGO<Player>("player");
	m_game = FindGO<Game>("game");
	m_difficultyLevel = FindGO<DifficultyLevel>("difficultyLevel");

	m_audioManager = FindGO<AudioManager>("audioManager");

	OnCollision();

	//========================
	// 難易度取得
	//========================
	m_difficultyLevel = FindGO<DifficultyLevel>("difficultyLevel");

	//========================
	// 難易度設定
	//========================
	if (m_game != nullptr)
	{
		switch (m_game->GetDifficulty())
		{
		case EASY:
			m_smallRobotHp = 50;
			m_smallRobotMaxHp = 50;
			m_attackPower = 5;
			m_knockBackPower = 1200.0f;
			break;

		case NORMAL:
			m_smallRobotHp = 75;
			m_smallRobotMaxHp = 75;
			m_attackPower = 10;
			m_knockBackPower = 800.0f;
			break;

		case HARD:
			m_smallRobotHp = 120;
			m_smallRobotMaxHp = 120;
			m_attackPower = 15;
			m_knockBackPower = 500.0f;
			break;

		case LUNATIC:
			m_smallRobotHp = 180;
			m_smallRobotMaxHp = 180;
			m_attackPower = 25;
			m_knockBackPower = 150.0f;
			break;
		}
	}

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
			m_collisionObject->SetIsEnable(false);
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

void SmallRobot::UpdateTimer()
{
	// 経過時間を取得
	float dt = g_gameTime->GetFrameDeltaTime();
	if (dt < 0.0f)
	{
		dt = 0.0f;
	}
}

void SmallRobot::KnockBack()
{
	if (!m_isKnockBack)
	{
		return;
	}

	m_knockBackTime -= g_gameTime->GetFrameDeltaTime();

	m_position = m_characterController.Execute(m_knockBackMove,2.0f / 60.0f);

	m_modelRender.SetPosition(m_position);

	// 徐々に減速
	m_knockBackMove *= 0.90f;

	if (m_knockBackTime <= 0.0f)
	{
		m_isKnockBack = false;
		m_knockBackMove = Vector3::Zero;
	}
}

void SmallRobot::Move()
{
	Vector3 playerPos = m_player->GetPosition();
	float distToPlayer = (playerPos - m_position).Length();

	if (distToPlayer <= 500.0f && m_timeCount == 0.0f && m_searchPlayer)
	{
		m_timeCount = 2.0f;
	}

	if (m_searchPlayer)
	{
		Vector3 forward = Vector3::Front;

		m_rotation.Apply(forward);

		forward.y = 0.0f;
		forward.Normalize();

		m_moveSpeed = forward * m_moveSpeedValue;
	}
	else
	{
		m_moveSpeed = Vector3::Zero;
	}

	m_position = m_characterController.Execute(m_moveSpeed, 2.0f / 60.0f);

	m_modelRender.SetPosition(m_position);
}

void SmallRobot::Rotation()
{
	if (!m_searchPlayer)
	{
		return;
	}
	
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	toPlayer.y = 0.0f;
	if (toPlayer.Length() < 0.001f)
	{
		return;
	}

	toPlayer.Normalize();

	Quaternion targetRot;
	targetRot.SetRotationYFromDirectionXZ(toPlayer);

	float t = m_rotationSpeed * g_gameTime->GetFrameDeltaTime();

	if (t > 1.0f)
	{
		t = 1.0f;
	}

	m_rotation.Slerp(t, m_rotation, targetRot);

	m_modelRender.SetRotation(m_rotation);
}

void SmallRobot::SearchPlayer()
{
	// 一度見つけたら永久追跡
	if (m_hasDetectedPlayer)
	{
		m_searchPlayer = true;
		return;
	}

	m_searchPlayer = false;

	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);

	Vector3 playerPos = m_player->GetPosition();
	Vector3 diff = playerPos - m_position;

	if (diff.Length() <= 2000.0f)
	{
		diff.Normalize();

		float angle = acosf(diff.Dot(m_forward));

		if (fabsf(angle) <= Math::PI * 0.15f)
		{
			m_searchPlayer = true;
			m_hasDetectedPlayer = true;
		}
	}
}

void SmallRobot::Attack()
{
    if (!m_isAttack)
    {
        return;
    }

    m_forward = Vector3::Front;
    m_rotation.Apply(m_forward);

    Vector3 pos = m_position + m_forward * 250.0f;

    m_collisionObject->SetPosition(pos);

    // ←追加
    m_collisionObject->SetIsEnable(true);

    m_attackCollisionLife = 0.1f;
    m_isAttack = false;
}

void SmallRobot::OnCollision()
{
	m_collisionObject = NewGO<CollisionObject>(0);

	m_collisionObject->CreateSphere(Vector3::Zero, Quaternion::Identity, 200.0f);

	m_collisionObject->SetName("smallRobotAttack");
	m_collisionObject->SetIsEnableAutoDelete(false);

	m_attackCollisionLife = 0.0f;
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
		if (!collision->IsHit(m_characterController))
		{
			continue;
		}

		if (m_damageIntarvalTime > 0.0f)
		{
			continue;
		}

		ReceiveAttack(false);
	}

	const auto& tackleCollisions = g_collisionObjectManager->FindCollisionObjects("playerTackle");

	for (auto collision : tackleCollisions)
	{
		if (!collision->IsHit(m_characterController))
		{
			continue;
		}

		if (m_damageIntarvalTime > 0.0f)
		{
			continue;
		}

		ReceiveAttack(true);
	}
}

void SmallRobot::ReceiveAttack(bool isTackle)
{
	int damage = m_player->GetAttackPower();
	damage = CalcDamage(damage);

	// タックルならダメージを下げる
	if (isTackle)
	{
		damage *= 0.6f; // ←ここが重要（好みで調整）
	}

	m_smallRobotHp -= damage;
	if (m_smallRobotHp < 0)
	{
		m_smallRobotHp = 0;
	}

	m_hasDetectedPlayer = true;

	ApplyKnockBack(isTackle);

	CreateDamageText(damage);

	m_damageIntarvalTime = 2.0f;
}

int SmallRobot::CalcDamage(int damage)
{
	int randomNum = rand() % 100 + 1;

	bool isCritical = randomNum <= 5;

	if (!m_searchPlayer)
	{
		damage *= isCritical ? 3 : 1.5f;

		PlayHitSE(isCritical);
	}
	else
	{
		damage *= isCritical ? 1.5f : 1.0f;
		PlayHitSE(isCritical);
	}

	return damage;
}

void SmallRobot::ApplyKnockBack(bool isTackle)
{
	Vector3 dir = m_position - m_player->GetPosition();
	dir.y = 0.0f;

	if (dir.LengthSq() > 0.001f)
	{
		dir.Normalize();
	}

	float power = m_knockBackPower;

	if (isTackle)
	{
		power *= 2.0f;   // タックルは強ノックバック
	}
	else
	{
		power *= 1.0f;   // 通常
	}

	m_knockBackMove = dir * power;
	m_isKnockBack = true;
	m_knockBackTime = 0.2f;
}

void SmallRobot::PlayHitSE(bool isCritical)
{
	bool isHit = m_player->GetAttackHit();

	if (isHit)
	{
		return;
	}

	m_player->SetAttackHit(true);

	if (!m_searchPlayer)
	{
		m_audioManager->PlaySE(enSound_BackstabSE, 1.0f, enSEPlay_AllowOverlap);

		return;
	}

	if (isCritical)
	{
		m_audioManager->PlaySE(enSound_CriticalSE, 1.0f, enSEPlay_AllowOverlap);

		return;
	}

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

void SmallRobot::CreateDamageText(int damage)
{
	DamageText* damageText = NewGO<DamageText>(0);

	Vector3 textPos = m_position;
	textPos.y += 250.0f;

	damageText->SetPosition(textPos);
	damageText->SetDamage(damage);
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
	float Wari = max(0.0f, (float)nowHP / (float)MaxHP);
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
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;

	EffectManager::GetInstance().PlayEffect(EffectManager::enEffect_Explosion,effectPos,50.0f);
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