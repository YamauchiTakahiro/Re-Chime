#pragma once
#include "Source/UIBase/UIBase.h"
class Load :public UIBase
{
public:
	Load();
	~Load();
	virtual bool Start() override;
	virtual void Update() override;
	virtual void Render(RenderContext& rc)override;

private:
	int LoadTime;
	Quaternion m_GearRotation;
	float m_GearRotSpeed;
	float m_time;
	FontRender m_LoadFont;
};

