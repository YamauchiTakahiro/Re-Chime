#include "stdafx.h"
#include "Potion.h"

Potion::Potion()
{
}

Potion::~Potion()
{
}

bool Potion::Start()
{
	return true;
}

void Potion::Update()
{
	Move();
	Rotation();
}

void Potion::Move()
{
}

void Potion::Rotation()
{
}

void Potion::Render(RenderContext& rc)
{
}