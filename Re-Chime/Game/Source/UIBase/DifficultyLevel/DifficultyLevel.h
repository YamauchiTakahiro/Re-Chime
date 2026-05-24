#pragma once
#include "Source/UIBase/UIBase.h"

class DifficultyLevel : public UIBase
{
public:
	enum Difficulty
	{
		EASY,
		NORMAL,
		HARD,
		LUNATIC
	};
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
	Difficulty m_difficulty = EASY;
	SpriteRender m_DifficultyLevel;
	FontRender m_Easy;
	FontRender m_Normal;
	FontRender m_Hard;
	FontRender m_Lunatic;
};