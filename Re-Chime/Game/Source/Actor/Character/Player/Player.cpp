#include "stdafx.h"
#include "Player.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Game.h"
#include "Source/Manager/AudioManager/AudioManager.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "Source/UIBase/UI/UI.h"
#include "Source/Manager/EffectManager/EffectManager.h"

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
	m_animationClips[enAnimationClip_Guard].SetLoopFlag(true);
	m_animationClips[enAnimationClip_KnockBack].Load("Assets/animData/Player/playerKnockBack.tka");
	m_animationClips[enAnimationClip_KnockBack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Tackle].Load("Assets/animData/Player/playerTackle.tka");
	m_animationClips[enAnimationClip_Tackle].SetLoopFlag(false);	
	//モデルを初期化する。
	m_modelRender.Init("Assets/modelData/Player/player.tkm", m_animationClips, enAnimationClip_Num);
	m_characterController.Init(100.0f, 300.0f, m_position);
	m_game = FindGO<Game>("game");
	m_gire = FindGO<Gire>("gire");

	m_audioManager = FindGO<AudioManager>("audioManager");

	if (m_game != nullptr)
	{
		switch (m_game->GetDifficulty())
		{
		case EASY:
			m_stamina = 120.0f;
			m_maxStamina = 120.0f;
			m_staminaRegenRate = 25.0f;
			break;
		case NORMAL:
			m_stamina = 100.0f;
			m_maxStamina = 100.0f;
			m_staminaRegenRate = 20.0f;
			break;

		case HARD:
			m_stamina = 100.0f;
			m_maxStamina = 100.0f;
			m_staminaRegenRate = 15.0f;
			break;

		case LUNATIC:
			m_stamina = 100.0f;
			m_maxStamina = 100.0f;
			m_staminaRegenRate = 10.0f;
			break;
		}
	}

	m_modelRender.SetShadowCasterFlag(true);

	SetScale();

	return true;
}

void Player::Update()
{	
	if (m_game && m_game->IsHitStop())
	{
		return;
	}

	bool isFade = m_game->IsFade();
	bool IntroFlag = m_game->GetIntro();
	bool bossIntroFlag = m_game->GetBossIntro();

	if (isFade || IntroFlag || bossIntroFlag)
	{
		// 完全停止
		m_moveSpeed = Vector3::Zero;
		m_playerState = enPlayerState_Idle;

		PlayAnimation();
		m_modelRender.Update();
		return;
	}
	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);

	if (isPause || m_game->IsGameStop())
	{
		return;
	}

	isNearItem = false;
	m_canPickItem = false;	

	UpdateTimer();

	if (!m_guardFlag && !m_isAttack && !isFade && !IntroFlag && !bossIntroFlag)
	{
		Move();
		Rotation();
	}
	else
	{
		// フェード中は完全停止
		m_moveSpeed.x = 0.0f;
		m_moveSpeed.z = 0.0f;
	}

	Hit();

	GetGires();

	if (!isFade &&!IntroFlag &&!bossIntroFlag)
	{
		JumpAndGravity();

		Attack();

		Tackle();

		ManageState();
	}

	if (m_CollisionLife <= 0.0f &&
		m_collisionObject != nullptr)
	{
		DeleteGO(m_collisionObject);
		m_collisionObject = nullptr;
	}

	if (m_powerBuffTime <= 0.0f)
	{
		m_powerBuffFlag = false;
	}
	if (m_attackSpeedBuffTime <= 0.0f)
	{
		m_attackSpeedBuffFlag = false;
	}

	PlayAnimation();

	DamageIntarval();

	FootStep();

	if (m_playerState == enPlayerState_Tackle)
	{
		TackleMove();
	}

	m_modelRender.Update();
}

void Player::UpdateTimer()
{
	// 経過時間を取得
	float dt = g_gameTime->GetFrameDeltaTime();

	if (m_game)
	{
		dt *= m_game->GetTimeScale();
	}

	if (dt < 0.0f)
	{
		dt = 0.0f;
	}

	if (m_collisionObject != nullptr)
	{
		// 攻撃の当たり判定の寿命を計測
		m_CollisionLife -= dt;
	}
	//アイテムのクールタイムを計測
	m_itemUseCoolTime -= dt;
	// 攻撃開始からの時間を計測
	m_attackStartTime += dt;
	//攻撃のクールタイムを計測
	m_attackCoolTime -= dt;
	//ダメージを受けてからの無敵時間を計測
	m_damageIntarvalTime -= dt;
	//移動の足音の時間を計測
	m_footStepTime -= dt;
	m_fadeTime -= dt;
	m_powerBuffTime -= dt;
	m_attackSpeedBuffTime -= dt;
	// =========================
	// ガードクールタイム
	// =========================
	if (m_guardCoolTime > 0.0f)
	{
		m_guardCoolTime -= dt;

		if (m_guardCoolTime < 0.0f)
		{
			m_guardCoolTime = 0.0f;
		}
	}

	// =========================
	// ガード中の残り時間
	// =========================
	if (m_guardFlag)
	{
		m_guardTimeLimit -= dt;

		if (m_guardTimeLimit <= 0.0f)
		{
			m_guardTimeLimit = 0.0f;

			// ガード終了
			m_guardFlag = false;

			// ガードモーション終了
			m_playerState = enPlayerState_Idle;

			// クールタイム開始
			m_guardCoolTime = 3.0f;
		}
	}

	Vector3 moveInput;
	moveInput.x = g_pad[0]->GetLStickXF();
	moveInput.z = g_pad[0]->GetLStickYF();

	bool isDashing = g_pad[0]->IsPress(enButtonRB1) && moveInput.LengthSq() > 0.01f && m_canDash && !m_isJumpStart;

	if (isDashing)
	{
		m_stamina -= 30.0f * dt;
	}
	else
	{
		m_stamina += m_staminaRegenRate * dt;
	}

	if (m_stamina <= 0.0f)
	{
		m_stamina = 0.0f;
		m_canDash = false;
	}

	if (m_stamina >= 50.0f)
	{
		m_canDash = true;
	}

	if (m_stamina > m_maxStamina)
	{
		m_stamina = m_maxStamina;
	}


	m_tackleCoolTime -= dt;

	if (m_tackleCoolTime < 0.0f)
	{
		m_tackleCoolTime = 0.0f;
	}

	m_tackleInvincibleTime -= dt;

	if (m_tackleInvincibleTime < 0.0f)
	{
		m_tackleInvincibleTime = 0.0f;
	}
}

void Player::Move()
{
	if (m_isJumpStart && !m_hasJumped)
	{
		m_moveSpeed.x = 0.0f;
		m_moveSpeed.z = 0.0f;
	}

	else if (!m_isKnockBack && !m_isTackle)
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

		if (moveDir.LengthSq() > 0.0001f)
		{
			moveDir.Normalize();
		}

		if (g_pad[0]->IsPress(enButtonRB1) && m_canDash)
		{
			m_speed = 480.0f;
		}
		else
		{
			m_speed = 300.0f;
		}

		m_moveSpeed += moveDir * m_speed;
	}

	Vector3 finalMoveSpeed = Vector3::Zero;

	if (m_isKnockBack && !m_isTackle)
	{
		finalMoveSpeed = m_knockBack;

		m_knockBack *= 0.9f;

		if (m_knockBack.LengthSq() < 10.0f)
		{
			m_knockBack = Vector3::Zero;
			m_isKnockBack = false;
		}
	}
	else if (m_isTackle)
	{
		finalMoveSpeed = m_tackleVelocity;

		if (m_tackleVelocity.LengthSq() < 100.0f)
		{
			m_tackleVelocity = Vector3::Zero;
		}
	}
	else
	{
		finalMoveSpeed = m_moveSpeed;
	}

	finalMoveSpeed.y = m_moveSpeed.y;

	m_position = m_characterController.Execute(finalMoveSpeed, 2.0f / 60.0f);

	m_modelRender.SetPosition(m_position);
}

void Player::TackleMove()
{
	Vector3 dir = Vector3::Front;
	m_rotation.Apply(dir);

	if (m_tackleVelocity.LengthSq() == 0.0f)
	{
		dir.Normalize();

		m_rotation.SetRotationYFromDirectionXZ(dir);
		m_modelRender.SetRotation(m_rotation);

		m_tackleVelocity = dir * 800.0f;
	}
}

void Player::JumpAndGravity()
{
	if (m_characterController.IsOnGround())
	{
		if (!m_hasJumped)
		{
			m_moveSpeed.y = 0.0f;
		}
	}

	// ジャンプ準備中
	if (m_isJumpStart)
	{
		m_jumpDelay -= g_gameTime->GetFrameDeltaTime();

		// 遅延終了でジャンプ
		if (m_jumpDelay <= 0.0f && !m_hasJumped)
		{
			m_moveSpeed.y = 500.0f;

			m_hasJumped = true;
			m_isJumpStart = false;
		}
	}

	// 重力
	if (!m_characterController.IsOnGround())
	{
		m_moveSpeed.y -= 20.0f;
	}
}

void Player::Rotation()
{
	if (m_isKnockBack)
	{
		return;
	}

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
			// 歩き足音をランダム再生
			int r = rand() % 2;

			AudioID id;

			switch (r)
			{
			case 0: id = enSound_PlayerWalkSE1; break;
			case 1: id = enSound_PlayerWalkSE2; break;
			}

			m_audioManager->PlaySE(id,1.0f,enSEPlay_AllowOverlap);

			m_footStepTime = 0.5f;
		}

		break;

	case enPlayerState_Run:

		if (m_footStepTime <= 0.0f)
		{
			// ダッシュ足音をランダム再生
			int r = rand() % 3;

			AudioID id;

			switch (r)
			{
			case 0: id = enSound_PlayerDashSE1; break;
			case 1: id = enSound_PlayerDashSE2; break;
			case 2: id = enSound_PlayerDashSE3; break;
			}
			m_audioManager->PlaySE(id,1.0f,enSEPlay_AllowOverlap);

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
		int attakcPower = rand() % 5 + 10; // 攻撃力を5から15の範囲でランダムに決定
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

		// 0.2秒後に当たり判定生成
		if (m_attackStartTime >= 0.2f &&!m_hasCreatedCollision)
		{
			OnCollision(enCollisionType_Attack);

			m_hasCreatedCollision = true;
			if (m_attackSpeedBuffFlag)
			{
				m_attackCoolTime = 1.0f;
				m_attackCoolTimeMax = 1.0f;
			}
			else
			{
				m_attackCoolTime = 2.0f;
				m_attackCoolTimeMax = 2.0f;
			}
		}

		if (m_attackStartTime >= 0.45f &&!m_hasPlayedHitSE)
		{
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

				m_audioManager->PlaySE(id,1.0f,enSEPlay_AllowOverlap);
			}

			m_hasPlayedHitSE = true;
		}
	}
}

void Player::Tackle()
{
	if (m_playerState != enPlayerState_Tackle)
	{
		return;
	}

	if (m_isTackle && !m_isKnockBack)
	{
		if (!m_hasCreatedCollision)
		{
			int attakcPower = rand() % 5 + 10; // 攻撃力を5から15の範囲でランダムに決定
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

			OnCollision(enCollisionType_Tackle);

			m_hasCreatedCollision = true;
		}
	}
}

void Player::OnCollision(EnCollisionType type)
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

	float radius = 0.0f;
	float forwardOffset = 0.0f;
	float lifeTime = 0.0f;
	const char* collisionName = "";
		
	switch (type)
	{
	case enCollisionType_Attack:
		radius = 200.0f;
		forwardOffset = 350.0f;
		lifeTime = 0.1f;
		collisionName = "playerAttack";
		break;
	case enCollisionType_Tackle:
		radius = 300.0f;
		forwardOffset = 150.0f;
		lifeTime = 0.2f;
		collisionName = "playerTackle";
		break;
	}

	collisionPos += m_forward * forwardOffset;
	collisionPos.y += 50.0f; // 攻撃の当たり判定を少し上にずらす
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, radius);
	m_collisionObject->SetName(collisionName);

	m_CollisionLife = lifeTime;
}

void Player::TakeDamage(int damage, const Vector3& enemyPos)
{
	if (m_isKnockBack)
	{
		return;
	}

	if (m_tackleInvincibleTime > 0.0f)
	{
		return;
	}

	if (m_damageIntarvalTime > 0.0f)
	{
		return;
	}

	// =========================
	// ガード中
	// =========================
	if (m_guardFlag)
	{
		// ダメージ半減
		int guardDamage = damage / 2;

		m_playerHp -= guardDamage;

		m_damageIntarvalTime = 0.2f;

		m_audioManager->PlaySE(
			enSound_PlayerGuardSE,
			1.0f,
			enSEPlay_AllowOverlap
		);

		if (m_playerHp <= 0)
		{
			m_playerHp = 0;

			Game* game = FindGO<Game>("game");
			if (game)
			{
				game->StartSlowMotion(0.7f, 0.2f);
			}
		}

		// ガードしたのでノックバックしない
		return;
	}

	// =========================
	// 通常ダメージ
	// =========================

	m_audioManager->PlaySE(
		enSound_PlayerDamageSE,
		1.0f,
		enSEPlay_AllowOverlap
	);

	// ノックバックの計算
	Vector3 dir = m_position - enemyPos;
	dir.y = 0.0f;

	if (dir.LengthSq() < 0.01f)
	{
		dir.x = -m_forward.x;
		dir.y = 0.0f;
		dir.z = -m_forward.z;
	}
	else
	{
		dir.Normalize();
	}

	if (m_playerHp <= 0)
	{
		m_knockBackPower = 1200.0f;
	}
	else
	{
		m_knockBackPower = 500.0f;
	}

	m_knockBack = dir * m_knockBackPower;
	m_knockBack.y = 0.0f;

	m_isKnockBack = true;

	m_damageIntarvalTime = 2.0f;

	m_playerHp -= damage;

	if (m_playerHp <= 0)
	{
		m_playerHp = 0;

		Game* game = FindGO<Game>("game");
		if (game)
		{
			game->StartSlowMotion(0.7f, 0.2f);
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
			isNearItem = true;
			m_canPickItem = true;
			if (g_pad[0]->IsTrigger(enButtonA))
			{
				m_powerBuffPotionCount++;

				m_audioManager->PlaySE(enSound_PowerUPSE,1.0f,enSEPlay_AllowOverlap);

				auto ui = FindGO<UI>("ui");
				if (ui)
				{
					ui->ShowPickItem(L"攻撃力アップポーションを拾った");
				}

				DeleteGO(collision);

				return;
			}
		}
	}

	const auto& collisions2 = g_collisionObjectManager->FindCollisionObjects("attackSpeedBuffPotion");
	for (auto collision : collisions2)
	{
		if (collision->IsHit(m_characterController) == true)
		{
			isNearItem = true;
			m_canPickItem = true;
			if (g_pad[0]->IsTrigger(enButtonA))
			{
				m_attackSpeedPotionCount++;

				m_audioManager->PlaySE(enSound_AttackSpeedUPSE,1.0f,enSEPlay_AllowOverlap);

				auto ui = FindGO<UI>("ui");
				if (ui)
				{
					ui->ShowPickItem(L"攻撃短縮ポーションを拾った");
				}

				DeleteGO(collision);

				return;
			}
		}
	}

	const auto& collisions3 = g_collisionObjectManager->FindCollisionObjects("healPotion");
	for (auto collision : collisions3)
	{
		if (collision->IsHit(m_characterController) == true)
		{
			isNearItem = true;
			m_canPickItem = true;

			if (g_pad[0]->IsTrigger(enButtonA))
			{
				m_healPotionCount++;

				m_audioManager->PlaySE(enSound_HealSE,1.0f,enSEPlay_AllowOverlap);

				auto ui = FindGO<UI>("ui");
				if (ui)
				{
					ui->ShowPickItem(L"回復ポーションを拾った");
				}

				DeleteGO(collision);

				return;
			}
		}
	}
}

void Player::GetGires()
{
	m_gire = FindGO<Gire>("gire");

	if (m_gire == nullptr)
	{
		return;
	}
	Vector3 diff = m_gire->GetPosition() - m_position;

	if (diff.LengthSq() < 250.0f * 250.0f)
	{
		isNearItem = true;
		m_canPickItem = true;

		if (g_pad[0]->IsTrigger(enButtonA))
		{
			m_gireCount++;

			DeleteGO(m_gire);
			m_gire = nullptr;
			
			auto ui = FindGO<UI>("ui");
			if (ui)
			{
				ui->ShowPickItem(L"歯車を拾った");
			}

			if (m_gireCount == 4)
			{
				m_game->SetGameClearFlag(true);
			}
			else
			{
				auto ui = FindGO<UI>("ui");

				if (ui)
				{
					ui->ShowGoal(L"階段を上れ");
				}
			}
		}
	}
}

void Player::PlayerState()
{
	m_isDash = false;

	if (m_isKnockBack)
	{
		m_playerState = enPlayerState_KnockBack;
		return;
	}

	// ガード開始
	if (!g_pad[0]->IsPress(enButtonLB1))
	{
		m_guardInputReleased = true;
	}

	if (g_pad[0]->IsTrigger(enButtonLB1) &&
		!m_isAttack &&
		!m_isTackle &&
		!m_isKnockBack &&
		!m_guardFlag &&
		m_guardCoolTime <= 0.0f &&
		m_guardInputReleased &&
		m_characterController.IsOnGround())
	{
		m_guardFlag = true;
		m_guardInputReleased = false;

		m_playerState = enPlayerState_Guard;

		m_guardTimeLimit = 3.0f;

		return;
	}

	if (!isNearItem && g_pad[0]->IsTrigger(enButtonA) &&
		m_attackCoolTime <= 0.0f && !m_guardFlag)
	{
		m_playerState = enPlayerState_Attack;

		m_isAttack = true;

		m_attackStartTime = 0.0f;
		m_hasCreatedCollision = false;

		return;
	}

	const float tackleCost = 15.0f;

	if (g_pad[0]->IsTrigger(enButtonX) &&
		m_tackleCoolTime <= 0.0f &&
		!m_guardFlag &&
		m_stamina >= tackleCost)
	{
		if (m_moveSpeed.LengthSq() > 0.01f)
		{
			m_rotation.SetRotationYFromDirectionXZ(m_moveSpeed);
			m_modelRender.SetRotation(m_rotation);
		}

		m_stamina -= tackleCost;

		m_playerState = enPlayerState_Tackle;
		m_isTackle = true;

		m_hasCreatedCollision = false;
		m_tackleInvincibleTime = 1.0f;

		return;
	}

	if (!m_isKnockBack &&
		g_pad[0]->IsTrigger(enButtonB) &&
		m_characterController.IsOnGround())
	{
		m_playerState = enPlayerState_Jump;

		m_isJumpStart = true;
		m_hasJumped = false;

		m_jumpDelay = 1.0f;

		return;
	}

	if (fabsf(m_moveSpeed.x) >= 0.001f ||
		fabsf(m_moveSpeed.z) >= 0.001f)
	{
		if (g_pad[0]->IsPress(enButtonRB1) && m_canDash)
		{
			m_isDash = true;
			m_playerState = enPlayerState_Run;
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
		m_hasCreatedCollision = false;
		m_attackStartTime = 0.0f;
		m_enemyHitFlag = false;
		m_hasPlayedHitSE = false;

		PlayerState();
	}
}
void Player::TackleState()
{
	if (m_modelRender.IsPlayingAnimation() == false || m_isKnockBack)
	{
		m_hasCreatedCollision = false;
		m_enemyHitFlag = false;
		m_isTackle = false;
		m_tackleCoolTime = 3.0f;

		m_tackleVelocity = Vector3::Zero;

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
		PlayerState();
	}
}

void Player::GuardState()
{
	// LB1を離した
	if (!g_pad[0]->IsPress(enButtonLB1))
	{
		m_guardFlag = false;

		// ガードを終了した瞬間からクールタイム開始
		m_guardCoolTime = 3.0f;

		// ガード残り時間をリセット
		m_guardTimeLimit = 0.0f;

		// ガードモーション終了
		m_playerState = enPlayerState_Idle;

		return;
	}

	// ガード中は移動しない
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;
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
		IdleState();
		break;
	case enPlayerState_Jump:
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
	case enPlayerState_Tackle:
		TackleState();
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
		ChangeAnimation(enAnimationClip_Idle);
		return;
	}

	switch (m_playerState)
	{
	case enPlayerState_Idle:
		ChangeAnimation(enAnimationClip_Idle);
		break;
	case enPlayerState_Jump:
		ChangeAnimation(enAnimationClip_Jump);
		break;
	case enPlayerState_Walk:
		ChangeAnimation(enAnimationClip_Walk);
		break;
	case enPlayerState_Run:
		ChangeAnimation(enAnimationClip_Run);
		break;
	case enPlayerState_Attack:
		ChangeAnimation(enAnimationClip_Attack);
		break;
	case enPlayerState_Guard:
		ChangeAnimation(enAnimationClip_Guard);
		break;
	case enPlayerState_KnockBack:
		ChangeAnimation(enAnimationClip_KnockBack);
		break;
	case enPlayerState_Tackle:
		ChangeAnimation(enAnimationClip_Tackle);
		break;
	default:
		break;
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
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;

	EffectManager::GetInstance().PlayEffect(EffectManager::enEffect_Heal,effectPos,50.0f);
}

void Player::MakePowerBuffEffect()
{
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;

	EffectManager::GetInstance().PlayEffect(EffectManager::enEffect_PowerBuff,effectPos,10.0f);
}

void Player::MakeAttackSpeedBuffEffect()
{
	Vector3 effectPos = m_position;
	effectPos.y += 200.0f;

	EffectManager::GetInstance().PlayEffect(EffectManager::enEffect_AttackSpeedBuff,effectPos,10.0f);
}

bool Player::UseItem(int itemNo)
{
	switch (itemNo)
	{
	case 0: // 攻撃速度
		if (m_attackSpeedPotionCount > 0)
		{
			m_attackSpeedPotionCount--;

			m_attackSpeedBuffFlag = true;
			m_attackSpeedBuffTime = 20.0f;

			MakeAttackSpeedBuffEffect();

			return true;
		}
		break;

	case 1: // 攻撃力
		if (m_powerBuffPotionCount > 0)
		{
			m_powerBuffPotionCount--;

			m_powerBuffFlag = true;
			m_powerBuffTime = 20.0f;

			MakePowerBuffEffect();

			return true;
		}
		break;

	case 2: // 回復
		if (m_healPotionCount > 0)
		{
			m_playerHp += m_heal;

			if (m_playerHp > m_playerMaxHp)
			{
				m_playerHp = m_playerMaxHp;
			}

			m_healPotionCount--;

			MakeHealEffect();

			return true;
		}
		break;
	}

	return false;
}

void Player::ChangeAnimation(EnAnimationClip anim)
{
	if (m_currentAnim == anim)
	{
		return;
	}

	m_currentAnim = anim;
	m_modelRender.PlayAnimation(anim);
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

void Player::KnockBackOnly(const Vector3& enemyPos, float power)
{
	if (m_isKnockBack)
	{
		return;
	}

	// 敵からプレイヤーへ向かう方向
	Vector3 dir;

	dir.x = m_position.x - enemyPos.x;
	dir.y = 0.0f;
	dir.z = m_position.z - enemyPos.z;

	// 敵とプレイヤーがほぼ同じ位置の場合
	if (dir.LengthSq() < 0.01f)
	{
		// プレイヤーの向いている方向の逆へ飛ばす
		dir.x = -m_forward.x;
		dir.y = 0.0f;
		dir.z = -m_forward.z;
	}
	else
	{
		dir.Normalize();
	}

	// 敵から離れる方向にノックバック
	m_knockBack.x = dir.x * power;
	m_knockBack.y = 0.0f;
	m_knockBack.z = dir.z * power;

	m_isKnockBack = true;
}