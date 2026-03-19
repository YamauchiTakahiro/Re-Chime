#include "stdafx.h"
#include "SmallRobot.h"

SmallRobot::SmallRobot()
{
	
}

SmallRobot::~SmallRobot()
{

}

bool SmallRobot::Start()
{
	m_modelRender.Init("Assets/modelData/Enemy/smallRobot/smallRobot.tkm");
	return true;
}

void SmallRobot::Update()
{
	Move();
	Rotation();
	m_modelRender.Update();
}

void SmallRobot::Move()
{

}

void SmallRobot::Rotation()
{

}

Vector3 SmallRobot::GetPosition(Vector3 pos)
{
	return pos;
}

void SmallRobot::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}