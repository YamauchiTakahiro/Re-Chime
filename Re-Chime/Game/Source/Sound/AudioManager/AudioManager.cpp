#include "stdafx.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "Sound/SoundEngine.h"
#include "sound/SoundSource.h"
#include <algorithm>
#include <fstream>

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
}

bool AudioManager::Start()
{
	Init();
	LoadVolume();
	return true;
}

void AudioManager::Update()
{
	for (auto it = m_playingSE.begin(); it != m_playingSE.end();)
	{
		if (!it->se || !it->se->IsPlaying())
		{
			it = m_playingSE.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void AudioManager::Init()
{
	if (g_soundEngine == nullptr)
	{
		m_isReady = false;
		return;
	}

	m_isReady = true;

	LoadAll();

	for (int i = 0; i < SE_POOL_SIZE; ++i)
	{
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(enSound_Num); //仮のIDで初期化。再生する際にIDを変更する。
		m_sePool.push_back(se);
	}
}

void AudioManager::PlayBGM(AudioID id, float volume)
{
	if (!m_isReady)
	{
		return;
	}

	if (m_bgm)
	{
		DeleteGO(m_bgm);
		m_bgm = nullptr;
	}

	m_bgm = NewGO<SoundSource>(0);
	m_bgm->Init(id);
	m_bgm->SetVolume(volume * m_bgmVolume * m_masterVolume);
	m_bgm->Play(true);
}

void AudioManager::StopBGM()
{
	if (m_bgm)
	{
		DeleteGO(m_bgm);
		m_bgm = nullptr;
	}
}


void AudioManager::SetBGMVolume(float volume)
{
	m_bgmVolume = (volume < 0.0f) ? 0.0f : (volume > 1.0f ? 1.0f : volume);

	if (m_bgm)
	{
		m_bgm->SetVolume(m_bgmVolume * m_masterVolume);
	}
}

SoundSource* AudioManager::GetFreeSE()
{
	for (auto se : m_sePool)
	{
		if (!se->IsPlaying())
		{
			return se;
		}
	}
	return nullptr;
}

void AudioManager::PlaySE(AudioID id, float volume)
{
	SoundSource* se = GetFreeSE();

	if (!se)
	{
		return;
	}

	se->Init(id);
	se->SetVolume(volume * m_seVolume * m_masterVolume);
	se->Play(false);

	m_playingSE.push_back({ id, se });
}

void AudioManager::SetSEVolume(float volume)
{
	m_seVolume = Clamp(volume, 0.0f, 1.0f);

	for (auto& se : m_playingSE)
	{
		if (se.se)
		{
			se.se->SetVolume(m_seVolume * m_masterVolume);
		}
	}
}

void AudioManager::StopSE(AudioID id)
{
	for (auto it = m_playingSE.begin(); it != m_playingSE.end();)
	{
		if (it->id == id)
		{
			if (it->se)
			{
				it->se->Stop();
			}
			it = m_playingSE.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void AudioManager::LoadAll()
{
	//BGMの読み込み
	Load(enSound_TitleBGM, "Assets/Sound/BGM/TitleBGM.wav");
	Load(enSound_LoadBGM, "Assets/Sound/BGM/LoadBGM.wav");
	Load(enSound_StageBGM, "Assets/Sound/BGM/StageBGM.wav");
	Load(enSound_GameOverBGM, "Assets/Sound/BGM/GameOverBGM.wav");
	Load(enSound_GameClearBGM, "Assets/Sound/BGM/GameClearBGM.wav");

	//SEの読み込み
	Load(enSound_PlayerWalkSE, "Assets/Sound/SE/PlayerWalkSE.wav");
	Load(enSound_PlayerDashSE, "Assets/Sound/SE/PlayerDashSE.wav");
	Load(enSound_EnemyWalkSE, "Assets/Sound/SE/EnemyWalkSE.wav");
	Load(enSound_BossWalkSE, "Assets/Sound/SE/BossWalkSE.wav");
	Load(enSound_FloorBossWalkSE, "Assets/Sound/SE/FloorBossWalkSE.wav");
	Load(enSound_HealSE, "Assets/Sound/SE/HealSE.wav");
	Load(enSound_PowerUpSE, "Assets/Sound/SE/PowerUpSE.wav");
	Load(enSound_SpeedUpSE, "Assets/Sound/SE/SpeedUpSE.wav");
	Load(enSound_BellSE, "Assets/Sound/SE/BellSE.wav");
	Load(enSound_GearDropSE, "Assets/Sound/SE/GearDropSE.wav");
	Load(enSound_EnemyDeathSE, "Assets/Sound/SE/EnemyDeathSE.wav");
	Load(enSound_BrokenBarricadeSE, "Assets/Sound/SE/BrokenBarricadeSE.wav");
	Load(enSound_PlayerDamageSE, "Assets/Sound/SE/PlayerDamageSE.wav");
	Load(enSound_PlayerAttackSE, "Assets/Sound/SE/PlayerAttackSE.wav");
	Load(enSound_PlayerGuardSE, "Assets/Sound/SE/PlayerGuardSE.wav");
}

void AudioManager::Load(AudioID id, const std::string& path)
{
	if (!m_isReady)
	{
		return;
	}

	if (id < 0 || id >= enSound_Num)
	{
		return;
	}
	g_soundEngine->ResistWaveFileBank(id, path.c_str());
}

void AudioManager::SaveVolume()
{
	std::ofstream file("volume.dat");

	if (!file.is_open()) return;

	file << m_masterVolume << std::endl;
	file << m_bgmVolume << std::endl;
	file << m_seVolume << std::endl;

	file.close();
}

void AudioManager::LoadVolume()
{
	std::ifstream file("volume.dat");

	if (!file.is_open()) return;

	file >> m_masterVolume;
	file >> m_bgmVolume;
	file >> m_seVolume;

	file.close();
}