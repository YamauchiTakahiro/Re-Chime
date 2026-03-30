#pragma once
#include "Source/UIBase/UIBase.h"
class Game;
class Player;
class SmallRobot;
class GameCamera;
class UI;
class DifficultyLevel :public UIBase
{
public:
	DifficultyLevel();
	~DifficultyLevel();
	virtual bool Start()override;
	virtual void Update()override;
	virtual void Render(RenderContext& rc)override;

private:
	Game* m_game = nullptr;
	Player* m_player = nullptr;
	SmallRobot* m_smallRobot = nullptr;
	GameCamera* m_gameCamera = nullptr;
	UI* m_ui = nullptr;
};