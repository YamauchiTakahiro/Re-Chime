#include "stdafx.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Source/Actor/Character/Player/Player.h"
#include "collision/CollisionObject.h"

Gire::Gire()
{

}

Gire::~Gire()
{
	DeleteGO(m_collisionObject);
}

bool Gire::Start()
{
	m_modelRender.Init("Assets/modelData/Item/Gear/Gear.tkm");
	m_player = FindGO<Player>("player");
	return true;
}

void Gire::Update()
{
	Move();

	bool isGetGire = m_player->GetGier();
	if (isGetGire == true)
	{
		GetGier();
	}
	m_modelRender.Update();
}

void Gire::Move()
{
	m_modelRender.SetPosition(m_position);
}

void Gire::GetGier()
{
	m_player->addGireCount();
	DeleteGO(this);
}

void Gire::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}