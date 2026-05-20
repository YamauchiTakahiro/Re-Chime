#pragma once

//ばねカメラを使用したい場合は、SpringCameraをインクルードする。
#include "camera/SpringCamera.h"

//クラス宣言。
class Player;
class Game;
class FinalBoss;

class GameCamera : public IGameObject
{
public:
	enum class EnCameraState
	{
		Intro,		 // 開始演出
		Blend,		 // ブレンド
		Normal,	 // 通常
		BossStart, // ボス登場演出
	};

	GameCamera();
	~GameCamera();
	bool Start();
	void Update();
	void UpdateIntroCamera();
	void UpdateBlendCamera();
	void UpdateNormalCamera();
	void UpdateBossCamera();
	void CameraState();
	void CameraTransition();
	void ResetCameraTimer()
	{
		m_introCameraTime = 0.0f;
		m_bossCameraTime = 0.0f;
	}

	/////////////////////////////////////
	//メンバ変数
	/////////////////////////////////////
private:
	Player* m_player = nullptr;		//プレイヤー。
	Vector3 m_toCameraPos = Vector3::One;
	SpringCamera m_springCamera;	//ばねカメラ。
	Game* m_game = nullptr;
	FinalBoss* m_finalBoss = nullptr;	//最終ボス。

	Vector3 m_firstFloorCenter = Vector3(0.0f, 0.0f, 0.0f);		//1階のイントロカメラの開始位置。
	Vector3 m_secondFloorCenter = Vector3(0.0f, 2137.0f, 0.0f);	//2階のイントロカメラの開始位置。
	Vector3 m_thirdFloorCenter = Vector3(0.0f, 4285.0f, 0.0f);		//3階のイントロカメラの開始位置。
	Vector3 m_fourthFloorCenter = Vector3(0.0f, 6442.2f, 0.0f);	//4階のイントロカメラの開始位置。
	Vector3 m_blendStartPos;		//ブレンドの開始位置。
	Vector3 m_blendStartTarget;	//ブレンドの開始注視点。
	
	bool m_isIntroCamera = true;	//イントロカメラかどうかのフラグ。
	bool m_isBossCamera = false;	//ボス戦カメラかどうかのフラグ。

	float m_introCameraTime = 0.0f;	//イントロカメラの時間。
	float m_introEndTime = 6.0f;		//イントロカメラの終了時間。
	float m_bossCameraTime = 0.0f;	//ボス戦カメラの時間。
	float m_bossCameraEndTime = 6.0f;	//ボス戦カメラの終了時間。
	float m_blendTime = 0.0f;		//ブレンドの時間。
	float m_blendEndTime = 1.0f;		//ブレンドの終了時間。

	EnCameraState m_cameraState = EnCameraState::Intro;	//カメラの状態。
};

