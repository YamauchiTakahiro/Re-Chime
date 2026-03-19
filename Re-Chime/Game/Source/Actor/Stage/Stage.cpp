#include "stdafx.h"
#include "Stage.h"

Stage::Stage()
{
}

Stage::~Stage()
{
}

bool Stage::Start()
{
	m_modelRender.Init("Assets/modelData/mainStage/Stage.tkm");
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
	m_modelRender.Update();
	return true;
}

void Stage::Update()
{
}

void Stage::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}