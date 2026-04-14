#pragma once
#include "Source/Actor/Item/Item.h"

class Player;

class Gire : public Item
{
public:
	Gire();
	~Gire();
	bool Start();
	void Update()override;
	void Move()override;
	void GetGier();
	Vector3 GetPosition(Vector3 pos)
	{
		pos = m_position;
		return pos;
	}
	Vector3 SetPosition(Vector3 position)
	{
		m_position = position;
		return m_position;
	}
	void Render(RenderContext& rc)override;
private:
	ModelRender m_modelRender;
	Vector3 m_position;
	Player* m_player = nullptr;
};

