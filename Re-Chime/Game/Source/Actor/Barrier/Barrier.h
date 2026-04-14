#pragma once
#include "Source/Actor/Actor.h"

class Barrier : public Actor
{
	public:
	Barrier();
	~Barrier();
	bool Start() override;
	void Update() override;
	void Rotation();
	void SetPosition(Vector3 position)
	{
		m_position = position;
		m_modelRender.SetPosition(position);
	}
	void Render(RenderContext& rc)override;

private:
	ModelRender m_modelRender;
	Vector3 m_position;
	Quaternion m_rotation;
	PhysicsStaticObject m_physicsStaticObject;
};

