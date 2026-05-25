#include "stdafx.h"
#include "DamageText.h"

DamageText::DamageText()
{
}

DamageText::~DamageText()
{
}

bool DamageText::Start()
{
	return true;
}

void DamageText::Update()
{
	m_position.y += 2.0f;

	m_timer -= g_gameTime->GetFrameDeltaTime();

	if (m_timer <= 0.0f)
	{
		DeleteGO(this);
	}
}

void DamageText::Render(RenderContext& rc)
{
    Vector2 screenPos;

    g_camera3D->CalcScreenPositionFromWorldPosition(
        screenPos,
        m_position
    );

    wchar_t text[256];

    swprintf_s(text, L"%d", m_damage);

    m_font.SetText(text);

    m_font.SetPosition(Vector3(
        screenPos.x,
        screenPos.y,
        0.0f
    ));

    m_font.SetScale(1.2f);

    m_font.SetColor(g_vec4Red);

    m_font.Draw(rc);
}
