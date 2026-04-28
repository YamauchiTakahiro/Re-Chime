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
	m_fade.Init("Assets/Sprite/Pause.DDs", 1920.0f, 1080.0f);
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
	m_modelRender.Update();

	m_player = FindGO<Player>("Player");
	return true;
}

void Stage::Update()
{
	//Fade();
}

void Stage::Fade()
{
	if (m_player == nullptr) return;

	Vector3 playerPos = m_player->GetPosition();
	float distance = (playerPos - m_stairPos).Length();

	float t = 1.0f - (distance / m_showDistance);

	if (t < 0.0f) t = 0.0f;
	if (t < 1.0f) t = 1.0f;

	m_alpha = t;

	m_fade.SetMulColor({ 1.0f, 1.0f, 1.0f,m_alpha });
}

void Stage::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);

	if (m_alpha > 0.01f) {
		m_fade.Draw(rc);
	}
}