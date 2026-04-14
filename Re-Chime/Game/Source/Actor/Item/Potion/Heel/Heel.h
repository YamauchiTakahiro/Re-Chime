#pragma once
#include "Source/Actor/Item/Potion/Potion.h"
class Heel : public Potion
{
	public:
	Heel();
	~Heel();
	bool Start()override;
	void Update()override;
	virtual void Move()override;
	virtual void Rotation()override;
	virtual void Render(RenderContext& rc)override;
};

