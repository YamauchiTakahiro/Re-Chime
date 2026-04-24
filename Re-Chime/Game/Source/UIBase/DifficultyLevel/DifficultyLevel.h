#pragma once
#include "Source/UIBase/UIBase.h"
class Game;
class AudioManager;
class Title;
class DifficultyLevel :public UIBase
{
public:
	DifficultyLevel();
	~DifficultyLevel();
	virtual bool Start()override;
	virtual void Update()override;
	virtual void Render(RenderContext& rc)override;
	void SetTitle(Title* title)
	{
		m_title = title;
	}
private:
	Game* m_game = nullptr;
	float m_bgmVolume = 1.0f;
	float m_seVolume = 1.0f;
	float m_masterVolume = 1.0f;

	SpriteRender m_BGMBar;
	SpriteRender m_SEBar;
	SpriteRender m_MasterBar;

	FontRender m_BGMText;
	FontRender m_SEText;
	FontRender m_MasterText;

	FontRender m_MasterValueText;
	FontRender m_BGMValueText;
	FontRender m_SEValueText;

	float rightX = 700.0f;
	int m_select = 0; //マスター 0 ,BGM 1 , SE 2
	AudioManager* m_audioManager = nullptr;

	Title* m_title = nullptr;
};