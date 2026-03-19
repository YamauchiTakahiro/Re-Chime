#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"
class SmallRobot : public Enemy
{
public:
	SmallRobot();
	virtual ~SmallRobot();
	virtual bool Start() override;
	virtual void Update() override;
	virtual void Move() override;
	virtual void Rotation() override;
	virtual Vector3 GetPosition(Vector3) override;
	virtual int GetHP(int hp) override
	{
		hp = m_smallRobotHp;
		return hp;
	}
	virtual void Render(RenderContext& rc)override;

private:
	ModelRender m_modelRender;
	int m_smallRobotHp = 50;		//!<小型ロボットのHP。
};

