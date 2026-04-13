#pragma once

#include "Level3DRender/LevelRender.h"


class Player;
class GameCamera;
class Stage;
class UI;
class SmallRobot;
class GameOver;
class Gire;
class FloorBoss;
class DifficultyLevel;
class MediumRobot;
class FinalBoss;

class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Pause();
	void Render(RenderContext& rc);

private:
	LevelRender m_levelRender;
	Player* m_player = nullptr;
	GameCamera* m_gameCamera = nullptr;
	Stage* m_stage = nullptr;
	UI* m_ui = nullptr;
	FontRender m_gear;
	SmallRobot* m_smallRobot = nullptr;
	FloorBoss* m_floorBoss = nullptr;
	MediumRobot* m_mediumRobot = nullptr;
	FinalBoss* m_finalBoss = nullptr;
	GameOver* m_gameOver = nullptr;
	Gire* m_gire = nullptr;
	DifficultyLevel* m_difficul = nullptr;
	bool m_isPause = false;
};

