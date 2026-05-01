#pragma once
#include "Source/Actor/Actor.h"
class Player;
class Stage : public Actor
{
	public:
	Stage();
	~Stage();
	bool Start() override;
	void Update() override;
	void SetPosition(Vector3 position)
	{
		m_position = position;
		m_modelRender.SetPosition(position);
	}
	void Render(RenderContext& rc)override;

private:
	Vector3 m_position;
	ModelRender m_modelRender;
	PhysicsStaticObject m_physicsStaticObject;
};

