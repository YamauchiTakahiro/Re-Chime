#pragma once
#include "Source/Actor/Actor.h"
class Item : public Actor
{
public:
	Item();
	~Item();
	bool Start()override;
	void Update()override;
	virtual void Move();
	virtual void Render(RenderContext& rc)override;
private:
	Vector3 m_position;
};

