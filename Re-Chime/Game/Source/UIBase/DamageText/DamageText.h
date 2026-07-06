#pragma once
class DamageText : public IGameObject
{
public:
	DamageText();
	~DamageText();

	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}

	void SetDamage(int damage)
	{
		m_damage = damage;
	}

	void SetCritical(bool critical)
	{
		m_isCritical = critical;
	}

private:
	FontRender m_font;

	Vector3 m_position;

	int m_damage = 0;

	float m_timer = 1.0f;
	bool m_isCritical = false;
};

