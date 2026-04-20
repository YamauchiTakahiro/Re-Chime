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
	virtual void Rotation() override;
	virtual void Attack() override;
	virtual void OnCollision() override;
	virtual void Time() override;
	virtual void Hit() override;
	virtual void AttackHit();
	virtual void DamageIntarval() override;
	virtual void Dide() override;
	virtual Vector3 GetPosition()const override
	{
		return m_position;
	}
	virtual void SetPosition(Vector3) override
	{

	}
	virtual void SetScale(Vector3 scale)
	{
	}	
	virtual int GetHP(int) override;
	virtual void Render(RenderContext& rc)override;
private:
	Vector3 m_position;
};

