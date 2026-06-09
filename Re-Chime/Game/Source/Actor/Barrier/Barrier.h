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
	void SetRotation(Quaternion rotation)
	{
		m_rotation = rotation;
		m_modelRender.SetRotation(rotation);
	}
	void SetScale(Vector3 scale)
	{
		m_scale = scale;
		m_modelRender.SetScale(scale);
	}
	void Render(RenderContext& rc)override;

private:
	ModelRender m_modelRender;
	Vector3 m_position = Vector3::Zero;
	Quaternion m_rotation = Quaternion::Identity;
	Vector3 m_scale = Vector3::Zero;
	PhysicsStaticObject m_physicsStaticObject;
};

