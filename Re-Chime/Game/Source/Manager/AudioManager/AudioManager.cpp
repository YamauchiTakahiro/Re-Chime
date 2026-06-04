#include "stdafx.h"
#include "Source/Manager/AudioManager/AudioManager.h"
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

void AudioManager::StopBGM(AudioID id)
{
	for (auto it = m_playingBGM.begin(); it != m_playingBGM.end();)
	{
		if (it->id == id)
		{
			if (it->bgm)
			{
				it->bgm->Stop();
			}

			it = m_playingBGM.erase(it);
		}
		else
		{
			++it;
		}
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

void AudioManager::PlaySE(AudioID id, float volume, SEPlayType type)
{
	if (type == enSEPlay_NoOverlap)
	{
		if (IsPlayingSE(id))
		{
			return;
		}
	}

	SoundSource* se = NewGO<SoundSource>(0);

	se->Init(id);

	se->SetVolume(volume * m_seVolume * m_masterVolume);

	se->Play(false);

	m_playingSE.push_back({ id, se });
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

bool AudioManager::IsPlayingSE(AudioID id)
{
	for (auto& se : m_playingSE)
	{
		if (se.id == id)
		{
			if (se.se && se.se->IsPlaying())
			{
				return true;
			}
		}
	}
	return false;
}

void AudioManager::SetSEVolume(float volume)
{
	m_seVolume = Clamp(volume, 0.0f, 1.0f);

	//for (auto& se : m_playingSE)
	//{
	//	if (se.se)
	//	{
	//		se.se->SetVolume(m_seVolume * m_masterVolume);
	//	}
	//}
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
	Load(enSound_PlayerWalkSE1, "Assets/Sound/SE/PlayerWalkSE_01.wav");
	Load(enSound_PlayerWalkSE2, "Assets/Sound/SE/PlayerWalkSE_02.wav");
	Load(enSound_PlayerWalkSE3, "Assets/Sound/SE/PlayerWalkSE_03.wav");
	Load(enSound_PlayerDashSE1, "Assets/Sound/SE/PlayerDashSE_01.wav");
	Load(enSound_PlayerDashSE2, "Assets/Sound/SE/PlayerDashSE_02.wav");
	Load(enSound_PlayerDashSE3, "Assets/Sound/SE/PlayerDashSE_03.wav");
	Load(enSound_EnemyWalkSE, "Assets/Sound/SE/EnemyWalkSE.wav");
	Load(enSound_BossWalkSE, "Assets/Sound/SE/BossWalkSE.wav");
	Load(enSound_FloorBossWalkSE, "Assets/Sound/SE/FloorBossWalkSE.wav");
	Load(enSound_BossShotSE_01, "Assets/Sound/SE/BossShotSE_01.wav");
	Load(enSound_BossShotSE_02, "Assets/Sound/SE/BossShotSE_02.wav");
	Load(enSound_HealSE, "Assets/Sound/SE/HealSE.wav");
	Load(enSound_PowerUPSE, "Assets/Sound/SE/PowerUPSE.wav");
	Load(enSound_AttackSpeedUPSE, "Assets/Sound/SE/AttackSpeedUpSE.wav");
	Load(enSound_BellSE, "Assets/Sound/SE/BellSE.wav");
	Load(enSound_GearDropSE, "Assets/Sound/SE/GearDropSE.wav");
	Load(enSound_EnemyDeathSE, "Assets/Sound/SE/EnemyDeathSE.wav");
	Load(enSound_BrokenBarricadeSE, "Assets/Sound/SE/BrokenBarricadeSE.wav");
	Load(enSound_PlayerDamageSE, "Assets/Sound/SE/PlayerDamageSE.wav");
	Load(enSound_PlayerAttackSE_01, "Assets/Sound/SE/PlayerAttackSE_01.wav");
	Load(enSound_PlayerAttackSE_02, "Assets/Sound/SE/PlayerAttackSE_02.wav");
	Load(enSound_PlayerAttackSE_03, "Assets/Sound/SE/PlayerAttackSE_03.wav");
	Load(enSound_PlayerGuardSE, "Assets/Sound/SE/PlayerGuardSE.wav");
	Load(enSound_MissSE_01, "Assets/Sound/SE/MissSE_01.wav");
	Load(enSound_MissSE_02, "Assets/Sound/SE/MissSE_02.wav");
	Load(enSound_MissSE_03, "Assets/Sound/SE/MissSE_03.wav");
	Load(enSound_StairsSE, "Assets/Sound/SE/StairsSE.wav");
	Load(enSound_ItemDropSE, "Assets/Sound/SE/ItemDropSE.wav");
	Load(enSound_GetGearSE, "Assets/Sound/SE/GetGearSE.wav");
	Load(enSound_GetItemSE, "Assets/Sound/SE/GetItemSE.wav");
	Load(enSound_CriticalSE, "Assets/Sound/SE/CriticalSE.wav");
	Load(enSound_BackstabSE, "Assets/Sound/SE/BackstabSE.wav");
	Load(enSound_DecisionSE, "Assets/Sound/SE/DecisionSE.wav");
	Load(enSound_ChoiceSE, "Assets/Sound/SE/ChoiceSE.wav");
	Load(enSound_OpenSE, "Assets/Sound/SE/Open.wav");
	Load(enSound_CloseSE, "Assets/Sound/SE/Close.wav");
	Load(enSound_PauseSE, "Assets/Sound/SE/PauseSE.wav");
	Load(enSound_PauseCloseSE, "Assets/Sound/SE/PauseCloseSE.wav");
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