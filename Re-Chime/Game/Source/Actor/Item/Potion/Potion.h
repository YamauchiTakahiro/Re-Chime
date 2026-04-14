#pragma once
#include "Source/Actor/Item/Item.h"

class Potion : public Item
{
public:
	Potion();
	~Potion();
	bool Start()override;
	void Update()override;
	virtual void Move()override;
	virtual void Rotation()override;
	virtual void Render(RenderContext& rc)override;
};