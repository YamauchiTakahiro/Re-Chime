#pragma once

#include "Level3DRender/LevelRender.h"


class Player;
class GameCamera;
class Stage;
class Barrier;
class UI;
class SmallRobot;
class GameOver;
class Gire;
class FloorBoss;
class DifficultyLevel;
class MediumRobot;
class FinalBoss;
class Title;
class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Pause();
	void PauseRender();
	void FirstFloor();
	void SecondFloor();
	void ThirdFloor();
	void FourthFloor();
	void EnemyCount()
	{
		m_enemyCount--;
	}
	bool GetIsPause(bool isPause)
	{
		isPause = m_isPause;
		return m_isPause;
	}
	void Render(RenderContext& rc);
private:
	LevelRender m_levelRender;
	Player* m_player = nullptr;
	GameCamera* m_gameCamera = nullptr;
	Stage* m_stage = nullptr;
	Barrier* m_barrier = nullptr;
	UI* m_ui = nullptr;
	FontRender m_gear;
	std::vector<SmallRobot*> m_smallRobot;
	std::vector<FloorBoss*> m_floorBoss;
	std::vector<MediumRobot*> m_mediumRobot;
	std::vector<FinalBoss*> m_finalBoss;
	GameOver* m_gameOver = nullptr;
	Gire* m_gire = nullptr;
	DifficultyLevel* m_difficul = nullptr;
	SpriteRender m_font;
	SpriteRender m_Pause;
	FontRender m_TitleReturnText;
	FontRender m_PlayerReturnText;
	Title* m_title = nullptr;

	bool m_isTitleReturn = false;
	bool m_isPlayerReturn = false;
	bool m_isPause = false;
	int m_enemyCount = 0;		//!<敵の数。
};

