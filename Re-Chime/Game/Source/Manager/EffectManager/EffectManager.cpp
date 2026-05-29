#include "stdafx.h"
#include "Source/Manager/EffectManager/EffectManager.h"
#include "graphics/effect/EffectEmitter.h"

EffectManager& EffectManager::GetInstance()
{
	static EffectManager effectManager;
	return effectManager;
}

void EffectManager::Load()
{
	EffectEngine::GetInstance()->ResistEffect(
		enEffect_Heal,
		u"Assets/effect/efk/Heal.efk"
	);

	EffectEngine::GetInstance()->ResistEffect(
		enEffect_PowerBuff,
		u"Assets/effect/efk/PowerBuff.efk"
	);

	EffectEngine::GetInstance()->ResistEffect(
		enEffect_AttackSpeedBuff,
		u"Assets/effect/efk/AttackSpeedBuff.efk"
	);

	EffectEngine::GetInstance()->ResistEffect(
		enEffect_Explosion,
		u"Assets/effect/efk/Explosion.efk"
	);

	EffectEngine::GetInstance()->ResistEffect(
		enEffect_BossExplosion,
		u"Assets/effect/efk/BossExplosion.efk"
	);

	EffectEngine::GetInstance()->ResistEffect(
		enEffect_NoticeCircle,
		u"Assets/effect/efk/NoticeCircle.efk"
	);
}

void EffectManager::PlayEffect(
	int effectId,
	const Vector3& position,
	float scale
)
{
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);

	effectEmitter->Init(effectId);

	effectEmitter->SetScale(Vector3::One * scale);

	effectEmitter->SetPosition(position);

	effectEmitter->Play();
}