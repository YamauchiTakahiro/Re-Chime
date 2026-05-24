#include "stdafx.h"
#include "GameCamera.h"
#include "Game.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Actor/Character/Enemy/FinalBoss/FinalBoss.h"
#include "Fade.h"

float Clamp(float value, float min, float max)
{
	if (value < min)
	{
		return min;
	}

	if (value > max)
	{
		return max;
	}

	return value;
}
#include "Source/UIBase/UI/UI.h"

GameCamera::GameCamera()
{

}

GameCamera::~GameCamera()
{

}

bool GameCamera::Start()
{//注視点から視点までのベクトルを設定。
	m_toCameraPos.Set(0.0f, 100.0f, 800.0f);

	//プレイヤーのインスタンスを探す。
	m_player = FindGO<Player>("player");
	//m_fade = FindGO<Fade>("fade");
	m_finalBoss = FindGO<FinalBoss>("finalBoss");

	//ばねカメラの初期化。
	m_springCamera.Init(
		*g_camera3D,		//ばねカメラの処理を行うカメラを指定する。
		10000.0f,			//カメラの移動速度の最大値。
		true,				//カメラと地形とのあたり判定を取るかどうかのフラグ。trueだとあたり判定を行う。
		5.0f				//カメラに設定される球体コリジョンの半径。第３引数がtrueの時に有効になる。
	);

	//カメラのニアクリップとファークリップを設定する //おそらく近平面と遠平面
	g_camera3D->SetNear(1.0f);
	g_camera3D->SetFar(50000.0f);

	m_game = FindGO<Game>("game");
	return true;
}

void GameCamera::Update()
{
	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);

	if (isPause || m_game->IsGameStop())
	{
		return;
	}

	if (m_player == nullptr)
	{
		return;
	}

	// 追加
	m_isIntroCamera = m_game->GetIntro();
	m_isBossCamera = m_game->GetBossIntro();

	//カメラの状態を更新する。
	CameraState();

	//カメラの状態に応じて、カメラを更新する。
	CameraTransition();

	//カメラの更新。
	if (m_cameraState == EnCameraState::Normal)
	{
		m_springCamera.Update();
	}
}

void GameCamera::CameraState()
{
	if (m_cameraState == EnCameraState::FadeOut ||m_cameraState == EnCameraState::FadeIn)
	{
		return;
	}

	if (m_isIntroCamera)
	{
		m_cameraState = EnCameraState::Intro;
	}
	else if (m_isBossCamera)
	{
		m_cameraState = EnCameraState::BossStart;
	}
	else
	{
		m_cameraState = EnCameraState::Normal;
	}
}

void GameCamera::CameraTransition()
{
	switch (m_cameraState)
	{
	case EnCameraState::Intro:
		UpdateIntroCamera();
		break;

	case EnCameraState::FadeOut:
		UpdateFadeOutCamera();
		break;

	case EnCameraState::FadeIn:
		UpdateFadeInCamera();
		break;

	case EnCameraState::Normal:
		UpdateNormalCamera();
		break;

	case EnCameraState::BossStart:
		UpdateBossCamera();
		break;
	}
}

void GameCamera::UpdateIntroCamera()
{
	m_introCameraTime += g_gameTime->GetFrameDeltaTime();

	Vector3 center;
	int floorNo = m_game->GetFloorNo();
	if(floorNo == 1)
	{
		center = m_firstFloorCenter;
	}
	else if(floorNo == 2)
	{
		center = m_secondFloorCenter;
	}
	else if(floorNo == 3)
	{
		center = m_thirdFloorCenter;
	}

	Vector3 target = center;
	target.y += 280.0f;

	float angle = m_introCameraTime * 40.0f;

	float rad = Math::DegToRad(angle);

	float radius = 2000.0f;

	Vector3 pos;
	pos.x = center.x + sinf(rad) * radius;
	pos.z = center.z + cosf(rad) * radius;
	pos.y = center.y + 500.0f;

	g_camera3D->SetPosition(pos);
	g_camera3D->SetTarget(target);

	if (m_introCameraTime > m_introEndTime) 
	{
		m_isIntroCamera = false;
		m_game->SetIntro(false);

		m_isStartFade = false;

		m_cameraState = EnCameraState::FadeOut;
	}
}

void GameCamera::UpdateBossCamera()
{
	m_bossCameraTime += g_gameTime->GetFrameDeltaTime();
	Vector3 center;
	int floorNo = m_game->GetFloorNo();
	if (floorNo == 4) {
		center = m_finalBoss->GetPosition();
	}

	Vector3 target = center;
	target.y += 280.0f;
	float angle = 0.0f;

	float t = m_bossCameraTime / m_bossCameraEndTime;

	if (t < 0.7f)
	{
		angle = m_bossCameraTime * 40.0f;
	}
	else
	{
		angle = 180.0f;
	}

	// ←ここで変換
	float rad = Math::DegToRad(angle);

	t = Clamp(t, 0.0f, 1.0f);

	// イージング
	t = t * t * (3.0f - 2.0f * t);

	// 半径を徐々に小さく
	float radius =
		2000.0f - (1200.0f * t);
	Vector3 pos;
	pos.x = target.x + sinf(rad) * radius;
	pos.z = target.z + cosf(rad) * radius;
	float height =
		1200.0f - (700.0f * t);

	pos.y = target.y + height;
	g_camera3D->SetPosition(pos);
	g_camera3D->SetTarget(target);
	if (m_bossCameraTime > m_bossCameraEndTime) {
		m_isBossCamera = false;
		m_game->SetBossIntro(false);
	}
}

void GameCamera::UpdateNormalCamera()
{
	//カメラを更新。
	//注視点を計算する。
	Vector3 target = m_player->GetPosition();
	//プレイヤの足元からちょっと上を注視点とする。
	target.y += 280.0f;
	target += g_camera3D->GetForward() * 20.0f;

	Vector3 toCameraPosOld = m_toCameraPos;
	//パッドの入力を使ってカメラを回す。
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();
	//Y軸周りの回転
	Quaternion qRot;
	qRot.SetRotationDeg(Vector3::AxisY, 2.0f * x);
	qRot.Apply(m_toCameraPos);
	//X軸周りの回転。
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, m_toCameraPos);
	axisX.Normalize();
	qRot.SetRotationDeg(axisX, 2.0f * y);
	qRot.Apply(m_toCameraPos);
	//カメラの回転の上限をチェックする。
	//注視点から視点までのベクトルを正規化する。
	//正規化すると、ベクトルの大きさが１になる。
	//大きさが１になるということは、ベクトルから強さがなくなり、方向のみの情報となるということ。
	Vector3 toPosDir = m_toCameraPos;
	toPosDir.Normalize();
	if (toPosDir.y < -0.5f) {
		//カメラが上向きすぎ。
		m_toCameraPos = toCameraPosOld;
	}
	else if (toPosDir.y > 0.8f) {
		//カメラが下向きすぎ。
		m_toCameraPos = toCameraPosOld;
	}

	//視点を計算する。
	Vector3 pos = target + m_toCameraPos;

	//バネカメラに注視点と視点を設定する。
	m_springCamera.SetPosition(pos);
	m_springCamera.SetTarget(target);
}

void GameCamera::UpdateFadeOutCamera()
{
	if (m_fade == nullptr)
	{
		m_fade = FindGO<Fade>("fade");

		if (m_fade == nullptr)
		{
			return;
		}
	}

	if (!m_isStartFade)
	{
		m_fade->StartFadeOut();

		m_isStartFade = true;
	}

	if (m_fade->IsFadeOutFinished())
	{
		Vector3 target = m_player->GetPosition();

		target.y += 280.0f;

		Vector3 pos = target + m_toCameraPos;

		g_camera3D->SetPosition(pos);
		g_camera3D->SetTarget(target);

		m_fade->StartFadeIn();

		m_cameraState = EnCameraState::FadeIn;

		m_isStartFade = false;
	}
}

void GameCamera::UpdateFadeInCamera()
{
	if (!m_fade->IsFade())
	{
		m_cameraState = EnCameraState::Normal;
	}
}