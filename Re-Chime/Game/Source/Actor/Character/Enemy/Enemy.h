#pragma once
#include "Source/Actor/Character/Character.h"
class Enemy :
    public Character
{
    public:
    Enemy();
    virtual ~Enemy();
    virtual bool Start() override;
    virtual void Update() override;
    virtual void Move() override;
	virtual void Render(RenderContext& rc)override;
};

