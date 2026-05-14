#pragma once
#include "Source/Actor/Item/Item.h"

class Player;
class AudioManager;

class Gire : public Item
{
public:
	Gire();
	~Gire();
	bool Start();
	void Update()override;
	void Move()override;
	void GetGier();
	Vector3 GetPosition()const	
	{
		return m_position;
	}
	Vector3 SetPosition(Vector3 position)
	{
		m_position = position;
		return m_position;
	}
	void SetScale();
	void Render(RenderContext& rc)override;
private:
	ModelRender m_modelRender;
	CollisionObject* m_collisionObject = nullptr;
	Vector3 m_position;
	Vector3 m_scale;
	Player* m_player = nullptr;
	AudioManager* m_audioManager;
};

