#include "stdafx.h"
#include "MediumRobot.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "Source/Manager/AudioManager/AudioManager.h"
#include "collision/CollisionObject.h"
#include "Game.h"
#include "Source/UIBase/DamageText/DamageText.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/Manager/EffectManager/EffectManager.h"

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

	//========================
	// 難易度設定
	//========================
	if (m_game != nullptr)
	{
		switch (m_game->GetDifficulty())
		{
		case EASY:
			m_mediumRobotHp = 50;
			m_mediumRobotMaxHp = 50;
			m_attackPower = 5;
			m_knockBackPower = 1200.0f;
			m_moveSpeedValue = 250.0f;
			m_rotationSpeed = 1.0f;
			break;

		case NORMAL:
			m_mediumRobotHp = 75;
			m_mediumRobotMaxHp = 75;
			m_attackPower = 10;
			m_knockBackPower = 800.0f;
			m_moveSpeedValue = 300.0f;
			m_rotationSpeed = 1.5f;
			break;

		case HARD:
			m_mediumRobotHp = 120;
			m_mediumRobotMaxHp = 120;
			m_attackPower = 15;
			m_knockBackPower = 500.0f;
			m_moveSpeedValue = 400.0f;
			m_rotationSpeed = 2.0f;
			break;

		case LUNATIC:
			m_mediumRobotHp = 180;
			m_mediumRobotMaxHp = 180;
			m_attackPower = 25;
			m_knockBackPower = 200.0f;
			m_moveSpeedValue = 650.0f;
			m_rotationSpeed = 2.0f;
			break;
		}
	}

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

	MediumRobotHP();

	m_modelRender.Update();
}

void MediumRobot::KnockBack()
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

void MediumRobot::Move()
{
	Vector3 playerPos = m_player->GetPosition();
	float distToPlayer =
		(playerPos - m_position).Length();

	if (distToPlayer <= 500.0f &&
		m_timeCount == 0.0f &&
		m_searchPlayer)
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

	m_position =
		m_characterController.Execute(
			m_moveSpeed,
			2.0f / 60.0f
		);

	m_modelRender.SetPosition(m_position);
}

void MediumRobot::Rotation()
{
	if (!m_searchPlayer)
	{
		return;
	}

	Vector3 toPlayer =
		m_player->GetPosition() - m_position;

	toPlayer.y = 0.0f;

	if (toPlayer.LengthSq() < 0.001f)
	{
		return;
	}

	toPlayer.Normalize();

	Quaternion targetRot;
	targetRot.SetRotationYFromDirectionXZ(toPlayer);

	float t =
		m_rotationSpeed *
		g_gameTime->GetFrameDeltaTime();

	t = min(t, 1.0f);

	m_rotation.Slerp(
		t,
		m_rotation,
		targetRot
	);

	m_modelRender.SetRotation(m_rotation);
}

void MediumRobot::SearchPlayer()
{
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
	const auto& collisions =
		g_collisionObjectManager->FindCollisionObjects("playerAttack");

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

	const auto& tackleCollisions =
		g_collisionObjectManager->FindCollisionObjects("playerTackle");

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

void MediumRobot::ReceiveAttack(bool isTackle)
{
	int damage = m_player->GetAttackPower();
	damage = CalcDamage(damage);

	// タックルならダメージを下げる
	if (isTackle)
	{
		damage *= 0.6f; // ←ここが重要（好みで調整）
	}

	m_mediumRobotHp -= damage;
	if (m_mediumRobotHp < 0)
	{
		m_mediumRobotHp = 0;
	}

	m_hasDetectedPlayer = true;

	ApplyKnockBack(isTackle);

	CreateDamageText(damage);

	m_damageIntarvalTime = 2.0f;
}

int MediumRobot::CalcDamage(int damage)
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

void MediumRobot::ApplyKnockBack(bool isTackle)
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

void MediumRobot::PlayHitSE(bool isCritical)
{
	bool isHit = m_player->GetAttackHit();

	if (isHit)
	{
		return;
	}

	m_player->SetAttackHit(true);

	if (!m_searchPlayer)
	{
		m_audioManager->PlaySE(
			enSound_BackstabSE,
			1.0f,
			enSEPlay_AllowOverlap
		);

		return;
	}

	if (isCritical)
	{
		m_audioManager->PlaySE(
			enSound_CriticalSE,
			1.0f,
			enSEPlay_AllowOverlap
		);

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

	m_audioManager->PlaySE(
		id,
		1.0f,
		enSEPlay_AllowOverlap
	);
}

void MediumRobot::CreateDamageText(int damage)
{
	DamageText* damageText = NewGO<DamageText>(0);

	Vector3 textPos = m_position;
	textPos.y += 250.0f;

	damageText->SetPosition(textPos);
	damageText->SetDamage(damage);
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
	//爆発エフェクトの生成
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;

	EffectManager::GetInstance().PlayEffect(
		EffectManager::enEffect_Explosion,
		effectPos,
		50.0f
	);
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
	float Wari = max(0.0f, (float)nowHP / (float)MaxHP);
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
