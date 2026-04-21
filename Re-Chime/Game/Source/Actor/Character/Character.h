#pragma once
#include "Source/Actor/Actor.h"
#include <graphics/RenderContext.h>
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
	virtual void Hit();
	virtual void DamageIntarval();
	virtual void Dide();
	virtual Vector3 GetPosition()const
		{
		return m_position;
	}
	virtual void SetPosition(Vector3)
	{

	}
	virtual void SetScale(Vector3 scale)
	{
	}
	virtual int GetHP(int);
	virtual void Render(RenderContext& rc)override;

private:
	Vector3 m_position;
};

