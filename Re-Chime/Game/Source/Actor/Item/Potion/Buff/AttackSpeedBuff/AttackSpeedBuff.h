#pragma once
#include "Source/Actor/Item/Potion/Buff/Buff.h"

class Player;
class AudioManager;

class AttackSpeedBuff : public Buff
{
public:
	AttackSpeedBuff();
	~AttackSpeedBuff();
	bool Start()override;
	void Update()override;
	void Move()override;
	void Rotation()override;
	void Collision();
	void SetScale();
	void Delete();
	void SetPosition(Vector3 pos)
	{
		m_position = pos;
		pos.y += 50.0f;
	}
	void Render(RenderContext& rc)override;

private:
	ModelRender m_modelRender;
	CollisionObject* m_collisionObject;
	Player* m_player;
	AudioManager* m_audioManager;
	Vector3 m_position;
};

