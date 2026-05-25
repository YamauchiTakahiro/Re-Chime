#pragma once
#include "Source/UIBase/UIBase.h"
#include "Difficulty.h"

class DifficultyLevel : public UIBase
{
public:
	DifficultyLevel();
	~DifficultyLevel();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;
	void DifficultySelect();
	Difficulty GetDifficulty()
	{
		return m_difficulty;
	}

private:
	int m_select = 0; //イージー 0 ,ノーマル 1 , ハード 2, ルナティック 3
	Difficulty m_difficulty;
	SpriteRender m_DifficultyLevel;

	SpriteRender m_Easy;
	SpriteRender m_Normal;
	SpriteRender m_Hard;
	SpriteRender m_Lunatic;

	FontRender m_Cursor;

	float m_animTime = 0.0f;
};