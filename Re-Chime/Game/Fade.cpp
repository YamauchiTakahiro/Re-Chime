#include "stdafx.h"
#include "Fade.h"

namespace
{
	const Vector3 SCALE = Vector3(2.16f, 2.16, 1.0f);
	const Vector3 POSITION = Vector3(0.0f, 0.0f, 0.0f);
}

Fade::Fade()
{

}

Fade::~Fade()
{

}

bool Fade::Start()
{
	m_fade.Init("Assets/Sprite/pause.DDs", 1920.0f, 1080.0f);

	m_fade.SetScale(SCALE);
	m_fade.SetPosition(POSITION);
	m_fade.Update();
	return true;
}

void Fade::Update()
{
	float delta = g_gameTime->GetFrameDeltaTime();

	switch (m_state) {
	case enState_FadeIn:
		m_currentAlpha -= m_fadeSpeed * delta;
		if (m_currentAlpha <= 0.0f) {
			m_currentAlpha = 0.0f;
			m_state = enState_Idle;
		}
		break;

	case enState_FadeOut:
		m_currentAlpha += m_fadeSpeed * delta;

		if (m_currentAlpha >= 1.0f)
		{
			m_currentAlpha = 1.0f;

			m_isFadeOutFinished = true;

			m_state = enState_Idle;
		}
		break;

	case enState_Idle:
		break;
	}
	m_fade.Update();
}

void Fade::Render(RenderContext& rc)
{
	if (m_currentAlpha > 0.0f) {
		m_fade.SetMulColor({ 1.0f, 1.0f, 1.0f, m_currentAlpha });
		m_fade.Draw(rc);
	}
}