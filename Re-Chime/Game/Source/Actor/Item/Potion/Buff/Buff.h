#pragma once
#include "Source/Actor/Item/Potion/Potion.h"

class Buff : public Potion
{
public:
	Buff();
	~Buff();
	bool Start()override;
	void Update()override;
	virtual void Move()override;
	virtual void Rotation()override;
	virtual void Render(RenderContext& rc)override;
};