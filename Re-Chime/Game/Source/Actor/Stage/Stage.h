#pragma once
#include "Source/Actor/Actor.h"
class Stage : public Actor
{
	public:
	Stage();
	~Stage();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc)override;

private:
	ModelRender m_modelRender;
	PhysicsStaticObject m_physicsStaticObject;
};

