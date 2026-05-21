#pragma once
#include "Source/Actor/Actor.h"

class Player;

class Bullet : public Actor
{
public:
	//メンバ関数
	Bullet();
	~Bullet();
	//開始処理
	bool Start() override;
	//更新処理
	void Update() override;
	//移動処理
	void Move();
	//回転処理
	void Rotation();
	//拡大率設定処理
	void SetScale();
	//コリジョン生成処理
	void OnCollision();
	//当たり判定処理
	void Hit();
	//描画処理
	void Render(RenderContext& renderContext);

	void SetPosition(Vector3 position)
	{
		m_position = position;
		m_modelRender.SetPosition(position);
	}

	void SetStartPosition(Vector3 startPosition)
	{
		m_startPosition = startPosition;
	}

	void SetMoveSpeed(Vector3 moveSpeed)
	{
		m_moveSpeed = moveSpeed;
	}

private:
	//メンバ変数
	ModelRender m_modelRender;//モデルレンダー
	Vector3 m_position;//座標
	Vector3 m_moveSpeed;//移動速度
	Vector3 m_startPosition;
	Vector3 m_scale;//拡大率
	Quaternion m_rotation;//回転
	CollisionObject* m_collisionObject = nullptr; //コリジョンオブジェクト
	int m_attackPower = 20;//攻撃力

	Player* m_player = nullptr;//プレイヤー
};

