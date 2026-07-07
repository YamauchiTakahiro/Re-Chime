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

    g_camera3D->CalcScreenPositionFromWorldPosition(screenPos, m_position);

    m_font.SetPosition(Vector3(screenPos.x, screenPos.y, 0.0f));

    wchar_t text[256];

    if (m_isBackAttack && m_isCritical)
    {
        swprintf_s(text, L"BACK ATTACK!!\nCRITICAL!!\n%d", m_damage);
    }
    else if (m_isBackAttack)
    {
        swprintf_s(text, L"BACK ATTACK!!\n%d", m_damage);
    }
    else if (m_isCritical)
    {
        swprintf_s(text, L"CRITICAL!!\n%d", m_damage);
    }
    else
    {
        swprintf_s(text, L"%d", m_damage);
    }

    m_font.SetText(text);

    if (m_isBackAttack && m_isCritical)
    {
        m_font.SetScale(2.0f);
        m_font.SetColor(g_vec4Orange);
    }
    else if (m_isBackAttack)
    {
        m_font.SetScale(1.8f);
        m_font.SetColor(g_vec4Cyan);      // 水色
    }
    else if (m_isCritical)
    {
        m_font.SetScale(2.0f);
        m_font.SetColor(g_vec4Red);
    }
    else
    {
        m_font.SetScale(1.2f);
        m_font.SetColor(g_vec4Yellow);
    }

    m_font.Draw(rc);
}
