#pragma once
class UIBase : public IGameObject
{
public:
	virtual bool Start() override;
	virtual void Update() override;
	virtual void Render(RenderContext& rc)override;

protected:
	SpriteRender m_HPBar;
	SpriteRender m_HP;
	SpriteRender m_Title;
	SpriteRender m_Load;
	SpriteRender m_GameOver;
	SpriteRender m_GameClear;
	SpriteRender m_DifficultyLevel;
	SpriteRender m_gear;
	SpriteRender m_Bar;
	SpriteRender m_BarFrame;
};

