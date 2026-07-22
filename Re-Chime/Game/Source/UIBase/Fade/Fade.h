#pragma once
class Fade: public IGameObject
{
public:
	Fade();
	~Fade();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc)override;

	//フェードアウト（暗くなる）
	void StartFadeOut(float speed = 0.5f)
	{
		m_currentAlpha = 0.0f;
		m_fadeSpeed = speed;
		m_isFadeOutFinished = false;
		m_state = enState_FadeOut;
	}

	//フェードイン　（明るくなる）
	void StartFadeIn(float speed = 0.5f)
	{
		m_currentAlpha = 1.0f;
		m_fadeSpeed = speed;
		m_isFadeOutFinished = false;
		m_state = enState_FadeIn;
	}

	//フェード中か判定する
	bool IsFade() const
	{
		return m_state != enState_Idle;
	}

	//a取得
	float GetAlpha() const
	{
		return m_currentAlpha;
	}

	bool IsFadeIn() const
	{
		return m_state == enState_FadeIn;
	}

	bool IsFadeOutFinished() const
	{
		return m_isFadeOutFinished;
	}

	void ResetFadeOut()
	{
		m_isFadeOutFinished = false;
	}

	void SetAlpha(float alpha)
	{
		m_currentAlpha = alpha;
	}

private:
	enum EnState {
		enState_FadeIn,
		enState_FadeOut,
		enState_Idle,
	};

	SpriteRender m_fade;
	EnState m_state = enState_Idle;

	float m_currentAlpha = 0.0f;
	float m_fadeSpeed = 0.5f;

	bool m_isFadeOutFinished = false;
};

