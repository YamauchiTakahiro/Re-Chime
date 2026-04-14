#include "stdafx.h"
#include "Buff.h"

Buff::Buff()
{
}

Buff::~Buff()
{
}

bool Buff::Start()
{
	return true;
}

void Buff::Update()
{
	Move();
	Rotation();
}

void Buff::Move()
{
}

void Buff::Rotation()
{
}

void Buff::Render(RenderContext& rc)
{
}