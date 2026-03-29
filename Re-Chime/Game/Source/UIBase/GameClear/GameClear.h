#pragma once
#include "Source/UIBase/UIBase.h"
class GameClear : public UIBase
{
public:
	GameClear();
	~GameClear();
	virtual bool Start() override;
	virtual void Update() override;
	virtual void Render(RenderContext& rc) override;

private:
};

