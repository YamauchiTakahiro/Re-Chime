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
	virtual void Death() override;
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
	virtual int GetHP() const override
	{
		return m_hp;
	}
	virtual void Render(RenderContext& rc)override;

protected:
	bool CanUpdate() const;
	float m_hitStopTime = 0.0f;
private:
	Vector3 m_position;
	int m_hp;
public:
	void StartHitStop(float time)
	{
		m_hitStopTime = time;
	}

	bool IsHitStop() const
	{
		return m_hitStopTime > 0.0f;
	}

	void UpdateHitStop()
	{
		if (m_hitStopTime > 0.0f)
		{
			m_hitStopTime -= g_gameTime->GetFrameDeltaTime();

			if (m_hitStopTime < 0.0f)
			{
				m_hitStopTime = 0.0f;
			}
		}
	}
};

