#include "stdafx.h"
#include "Enemy.h"
#include "Game.h"

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
}

void Enemy::Move()
{
}

void Enemy::Rotation()
{
}

void Enemy::Attack()
{
}

void Enemy::OnCollision()
{
}

void Enemy::Time()
{
}

void Enemy::Hit()
{
}

void Enemy::AttackHit()
{
}

void Enemy::DamageIntarval()
{
}

void Enemy::Death()
{
}

void Enemy::Render(RenderContext& rc)
{
}

bool Enemy::CanUpdate() const
{
    Game* game = FindGO<Game>("game");

    if (game && game->IsHitStop())
    {
        return false;
    }

    return !game->IsHitStop();
}