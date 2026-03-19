#pragma once
#include "Source/Actor/Actor.h"
class Character : public Actor
{
public:
	Character();
	virtual ~Character();
	virtual bool Start() override;
	virtual void Update() override;
	virtual void Move();
	virtual void Rotation();
	virtual void Attack();
	virtual void OnCollision();
	virtual void Time();
	virtual Vector3 GetPosition(Vector3);
	virtual int GetHP(int);
	virtual void Render(RenderContext& rc)override;
};

