#pragma once

enum AudioID
{
	enSound_TitleBGM,
	enSound_LoadBGM,
	enSound_StageBGM,
	enSound_GameOverBGM,
	enSound_GameClearBGM,
	enSound_PlayerWalkSE1,
	enSound_PlayerWalkSE2,
	enSound_PlayerWalkSE3,
	enSound_PlayerDashSE1,
	enSound_PlayerDashSE2,
	enSound_PlayerDashSE3,
	enSound_EnemyWalkSE,
	enSound_BossWalkSE,
	enSound_FloorBossWalkSE,
	enSound_HealSE,
	enSound_BuffSE,
	enSound_BellSE,
	enSound_GearDropSE,
	enSound_EnemyDeathSE,
	enSound_BrokenBarricadeSE,
	enSound_PlayerDamageSE,
	enSound_PlayerAttackSE,
	enSound_PlayerGuardSE,
	enSound_Num //このステータスは、サウンドの総数を表しているため、この下には追加しないでください
};

enum SEPlayType
{
	enSEPlay_AllowOverlap,
	enSEPlay_NoOverlap,
};

struct SEHandle
{
	AudioID id;
	SoundSource* se;
};

class AudioManager : public IGameObject
{
public:
	AudioManager();
	~AudioManager();

	bool Start() override;
	void Update() override;

	void Init();

	void PlayBGM(AudioID id, float volume = 1.0f);
	void StopBGM();

	void PlaySE(AudioID id, float volume = 1.0f, SEPlayType type = enSEPlay_AllowOverlap);
	void StopSE(AudioID id);
	bool IsPlayingSE(AudioID id);

	void SetBGMVolume(float volume);
	void SetSEVolume(float volume);

	float GetMasterVolume() const { return m_masterVolume; }
	float GetBGMVolume() const { return m_bgmVolume; }
	float GetSEVolume() const { return m_seVolume; }
	void SetMasterVolume(float volume)
	{
		m_masterVolume = Clamp(volume, 0.0f, 1.0f);

		// BGM更新
		if (m_bgm)
		{
			m_bgm->SetVolume(m_bgmVolume * m_masterVolume);
		}

		// SE更新
		for (auto& se : m_playingSE)
		{
			if (se.se)
			{
				se.se->SetVolume(m_seVolume * m_masterVolume);
			}
		}
	}
	void SaveVolume();
	void LoadVolume();

private:
	void LoadAll();
	void Load(AudioID id, const std::string& path);

	SoundSource* GetFreeSE();

	float Clamp(float value, float min, float max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

private:
	bool m_isReady = false;

	SoundSource* m_bgm = nullptr;

	static const int SE_POOL_SIZE = 16;
	std::vector<SoundSource*> m_sePool;
	std::vector<SEHandle> m_playingSE;


	float m_bgmVolume = 1.0f;
	float m_seVolume = 1.0f;
	float m_masterVolume = 1.0f;

	// コピー禁止
	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
};