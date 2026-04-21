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
	SpriteRender m_Gear;
	SpriteRender m_gear;
	SpriteRender m_gear2;
	SpriteRender m_gear3;
	SpriteRender m_Bar;
	SpriteRender m_BarFrame;
	SpriteRender m_Abutton;
	SpriteRender m_Bbutton;
	SpriteRender m_Xbutton;
	SpriteRender m_Ybutton;
	SpriteRender m_UP;
	SpriteRender m_AttackSpeed;
	SpriteRender m_explanation;
	FontRender m_GireText;
};

