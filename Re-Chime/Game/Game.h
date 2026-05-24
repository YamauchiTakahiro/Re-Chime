#pragma once

#include "Level3DRender/LevelRender.h"
#include "sound/SoundSource.h"


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
class RareRobot;
class Title;
class GameClear;
class AudioManager;
class Heal;
class Fade;

class Game : public IGameObject
{
public:
	enum Difficulty
	{
		EASY,
		NORMAL,
		HARD,
		LUNATIC
	};
	Game();
	~Game();
	bool Start();
	void Update();
	void Pause();
	void PauseRender();
	void CreateGire();
	void FirstFloor();
	void SecondFloor();
	void ThirdFloor();
	void FourthFloor();
	bool IsFade() const;

	void SetDifficulty(Difficulty difficulty)
	{
		m_difficulty = difficulty;
	}

	Difficulty GetDifficulty() const
	{
		return m_difficulty;
	}

	float GetLoadProgress() const
	{
		if (m_maxLoadCount == 0)
		{
			return 0.0f;
		}

		return (float)m_loadCount / (float)m_maxLoadCount;
	}
	void EnemyCount()
	{
		m_numDefeatedEnemy++;
	}
	bool GetIsPause(bool isPause)
	{
		isPause = m_isPause;
		return m_isPause;
	}
	void SetSetting(bool flag)
	{
		m_isSetting = flag;
	}
	void SetDifficulty(DifficultyLevel* dl)
	{
		m_difficul = dl;
	}
	bool FadeFlag()
	{
		return m_isNear;
	}
	void SetGameStop(bool flag)
	{
		m_isGameStop = flag;
	}

	bool IsGameStop() const
	{
		return m_isGameStop;
	}

	int GetNeedGireCount() const
	{
		return m_needGireCount;
	}

	int GetFloorNo() const
	{
		return m_floorNo;
	}
	bool GetIntro() const
	{
		return m_intro;
	}
	bool SetIntro(bool intro)
	{
		m_intro = intro;
		return m_intro;
	}
	bool GetBossIntro() const
	{
		return m_bossIntro;
	}
	bool SetBossIntro(bool bossIntro)
	{
		m_bossIntro = bossIntro;
		return m_bossIntro;
	}

	bool IsReady() const
	{
		return m_isReady;
	}

	void SetLoading(bool flg)
	{
		m_isLoading = flg;
	}

	void Render(RenderContext& rc);
private:
	LevelRender m_levelRender;
	Player* m_player = nullptr;
	GameCamera* m_gameCamera = nullptr;
	Stage* m_stage = nullptr;
	Barrier* m_barrier1 = nullptr;
	Barrier* m_barrier2 = nullptr;
	Barrier* m_barrier3 = nullptr;
	Barrier* m_barrier4 = nullptr;
	Barrier* m_barrier5 = nullptr;
	Barrier* m_barrier6 = nullptr;
	UI* m_ui = nullptr;
	FontRender m_gear;
	std::vector<SmallRobot*> m_smallRobot;
	std::vector<FloorBoss*> m_floorBoss;
	std::vector<MediumRobot*> m_mediumRobot;
	std::vector<FinalBoss*> m_finalBoss;
	std::vector<RareRobot*> m_rareRobot;
	GameOver* m_gameOver = nullptr;
	Gire* m_gire = nullptr;
	DifficultyLevel* m_difficul = nullptr;
	Difficulty m_difficulty = NORMAL;
	SpriteRender m_font;
	SpriteRender m_Pause;
	FontRender m_Cursor;
	FontRender m_TitleReturnText;
	FontRender m_PlayerReturnText;
	FontRender m_SoundText;
	Title* m_title = nullptr;
	GameClear* m_gameClear = nullptr;
	AudioManager* m_audioManager = nullptr;
	Heal* m_heal = nullptr;
	Vector3 m_cursorPos;
	FontRender m_Pos;
	Fade* m_fade = nullptr;
	Vector3 m_stairMin = { 2350.0f - rangeX, 376.0f, 3300.0f };
	Vector3 m_stairMax = { 2350.0f + rangeX, 376.8f, 3900.0f };
	Vector3 m_nextMovePos;

	int m_pauseSelect = 0; //ポーズメニュー、0:タイトルに戻る、1:ゲームに戻る、2:音量調整
	bool m_isPause = false;
	bool m_isPlayerReturn = false;
	int m_enemyCount = 0;		//!<敵の数。
	int m_numDefeatedEnemy = 0;	//!<倒した敵の数。
	bool m_isSetting = false;
	bool m_intro = true;
	bool m_bossIntro = false;
	bool m_isMoveNextFloor = false;
	bool m_nextIntro = false;
	bool m_nextBossIntro = false;
	bool m_isReady = false;
	float m_pauseTime;

	bool m_isNear = false;
	float rangeX = 300.0f;
	float m_showDistance = 200.0f;
	struct FadeArea {
		Vector3 pos;
		float radius;
		Vector3 targetPos;
		bool intro;
		bool bossIntro;
	};

	std::vector<FadeArea> m_fadeAreas;
	bool m_createGire = false;
	bool m_isGameStop = false;

	enum EnGoalState
	{
		enGoalState_DefeatEnemy,
		enGoalState_GetGear,
		enGoalState_GoStairs,
	};

	EnGoalState m_goalState = enGoalState_DefeatEnemy;

	int m_floorNo = 1;
	int m_needGireCount = 1;
	int m_loadCount = 0;
	int m_maxLoadCount = 0;
	bool m_isLoading = true;
};

