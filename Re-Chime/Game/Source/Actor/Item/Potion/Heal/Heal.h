#pragma once
#include "Source/Actor/Item/Potion/Potion.h"

class Player;

class Heal : public Potion
{
	public:
	Heal();
	~Heal();
	bool Start()override;
	void Update()override;
	void Move()override;
	void Rotation()override;
	void Collision();
	void Delete();
	void SetPosition(Vector3 pos)
	{
		m_position = pos;
	}
	void Render(RenderContext& rc)override;

private:
	ModelRender m_modelRender;
	CollisionObject* m_collisionObject;
	Player* m_player;
	Vector3 m_position;
};

