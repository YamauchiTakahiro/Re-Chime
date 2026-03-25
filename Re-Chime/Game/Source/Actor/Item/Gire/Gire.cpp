#include "stdafx.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Source/Actor/Character/Player/Player.h"

Gire::Gire()
{

}

Gire::~Gire()
{

}

bool Gire::Start()
{
	m_modelRender.Init("Assets/modelData/Item/Gire/Gire.tkm");
	m_player = FindGO<Player>("player");
	return true;
}

void Gire::Update()
{
	Move();

	GetGier();
	m_modelRender.Update();
}

void Gire::Move()
{
	m_modelRender.SetPosition(m_position);
}

void Gire::GetGier()
{
	Vector3 playerPos;
	playerPos = m_player->GetPosition(playerPos);
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 200)
	{
		DeleteGO(this);
	}
}

void Gire::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}