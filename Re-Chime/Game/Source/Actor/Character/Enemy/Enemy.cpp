#include "stdafx.h"
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

bool Enemy::Start()
{
	return true;
}

void Enemy::Update()
{
	Move();
	Rotation();
}

void Enemy::Move()
{
}

void Enemy::Rotation()
{
}

Vector3 Enemy::GetPosition(Vector3 pos)
{
	return pos;
}

int Enemy::GetHP(int hp)
{
	return hp;
}

void Enemy::Render(RenderContext& rc)
{
}