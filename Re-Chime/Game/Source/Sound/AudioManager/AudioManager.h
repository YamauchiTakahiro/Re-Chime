#pragma once

enum AudioID
{
	enSound_TitleBGM,
	enSound_LoadBGM,
	enSound_StageBGM,
	enSound_GameOverBGM,
	enSound_GameClearBGM,
	enSound_PlayerWalkSE,
	enSound_PlayerDashSE,
	enSound_EnemyWalkSE,
	enSound_BossWalkSE,
	enSound_FloorBossWalkSE,
	enSound_HealSE,
	enSound_PowerUpSE,
	enSound_SpeedUpSE,
	enSound_BellSE,
	enSound_GearDropSE,
	enSound_EnemyDeathSE,
	enSound_BrokenBarricadeSE,
	enSound_PlayerDamageSE,
	enSound_PlayerAttackSE,
	enSound_PlayerGuardSE,
	enSound_Num //このステータスは、サウンドの総数を表しているため、この下には追加しないでください
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

	void PlaySE(AudioID id, float volume = 1.0f);
	void StopSE(AudioID id);

	void SetBGMVolume(float volume);
	void SetSEVolume(float volume);

	float GetBGMVolume() const;
	float GetSEVolume() const;

private:
	void LoadAll();
	void Load(AudioID id, const std::string& path);

	float m_bgmVolume = 1.0f;
	float m_seVolume = 1.0f;

	SoundSource* GetFreeSE();

private:
	bool m_isReady = false;

	SoundSource* m_bgm = nullptr;

	static const int SE_POOL_SIZE = 16;
	std::vector<SoundSource*> m_sePool;
	std::vector<SEHandle> m_playingSE;

	// コピー禁止
	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
};