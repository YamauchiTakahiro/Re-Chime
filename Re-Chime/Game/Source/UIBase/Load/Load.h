#pragma once
#include "Source/UIBase/UIBase.h"
#include "Difficulty.h"

class AudioManager;
class Game;
class Load :public UIBase
{
public:
	Load();
	~Load();
	virtual bool Start() override;
	virtual void Update() override;
	virtual void Render(RenderContext& rc)override;

	void SetDifficulty(Difficulty d)
	{
		m_difficulty = d;
	}

private:
	Difficulty m_difficulty = NORMAL;
	int LoadTime;
	Quaternion m_GearRotation;
	Quaternion m_GearRotation2;
	Quaternion m_GearRotation3;
	float m_GearRotSpeed;
	float m_loadTimer = 0.0f;
	float m_loadMax = 5.3f; // 5秒
	float m_dotTimer = 0.0f;
	float m_barCurrent = 0.0f;
	int m_dotCount = 0;
	bool m_isFirstRender = false;
	bool m_isCreateGame = false;
	Game* m_game = nullptr;
	FontRender m_Font;
	AudioManager* m_audioManager;
};

