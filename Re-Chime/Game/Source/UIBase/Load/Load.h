#pragma once
#include "Source/UIBase/UIBase.h"

class AudioManager;

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
	Quaternion m_GearRotation2;
	Quaternion m_GearRotation3;
	float m_GearRotSpeed;
	float m_loadTimer = 0.0f;
	float m_loadMax = 5.3f; // 5秒
	float m_dotTimer = 0.0f;
	int m_dotCount = 0;
	FontRender m_Font;
	AudioManager* m_audioManager;
};

