#include "stdafx.h"
#include "Barrier.h"

Barrier::Barrier()
{
}

Barrier::~Barrier()
{
}

bool Barrier::Start()
{
	m_modelRender.Init("Assets/modelData/mainStage/brarrier.tkm");
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	m_modelRender.Update();
	return true;
}

void Barrier::Rotation()
{
	m_rotation.SetRotationDegY(90.0f);
	m_modelRender.SetRotation(m_rotation);
}

void Barrier::Update()
{
	//Rotation();
	m_modelRender.Update();
}

void Barrier::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}