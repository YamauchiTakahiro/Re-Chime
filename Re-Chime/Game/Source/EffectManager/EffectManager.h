#pragma once

class EffectManager : public IGameObject
{
public:

	enum EnEffect
	{
		enEffect_Heal = 1,
		enEffect_PowerBuff,
		enEffect_AttackSpeedBuff,
		enEffect_Explosion,
		enEffect_BossExplosion,
	};

public:

	static EffectManager& GetInstance();

	void Load();

	void PlayEffect(
		int effectId,
		const Vector3 & position,
		float scale = 1.0f
	);

	EffectManager() = default;
	~EffectManager() = default;
	};

