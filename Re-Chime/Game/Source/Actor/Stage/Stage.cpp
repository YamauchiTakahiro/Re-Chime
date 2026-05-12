#include "stdafx.h"
#include "Stage.h"
#include "Source/Actor/Character/Player/Player.h"

Stage::Stage()
{
}

Stage::~Stage()
{
}

bool Stage::Start()
{
	m_modelRender.Init("Assets/modelData/mainStage/Stage.tkm");
	m_modelRender.SetShadowCasterFlag(true);
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
	
	return true;
}

void Stage::Update()
{
	m_modelRender.Update();
}

void Stage::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}