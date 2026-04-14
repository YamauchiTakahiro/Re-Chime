#pragma once
#include "Source/Actor/Character/Character.h"

class Gire;
class Game;
class Player : public Character
{
public:
    Player();
    ~Player();
    bool Start() override;
    void Update() override;
    void Move() override;
    void Rotation() override;
	void Attack() override;
	void OnCollision() override;
	void Time() override;
	void Hit() override;
	void DamageIntarval() override;
    void GuardCollision();
    void Gurad();
    /*void PlayerState();
	void PlayAnimation();*/
	void PowerBuff();
	void PowerBuffTime();
	void AttackSpeedBuffTime();
	void Heal();
    Vector3 GetPosition(Vector3 pos) override
    {
        pos = m_position;
        return pos;
    }
    int GetHP(int hp) override
    {
		hp = m_playerHp;
		return hp;
    }
    int GetMaxHP(int maxhp)
    {
        maxhp = m_playerMaxHp;
        return maxhp;
    }
    int GetAttackPower(int attackPower)
    {
        attackPower = m_attackPower;
        return attackPower;
	}
    bool GetGier(bool getGier)
    {
        getGier = m_isGetGire;
		return getGier;
    }
    void GetGireCount()
    {
        m_gireCount++;
    }
    bool GetPowerBuffFlag(bool powerBuffFlag)
    {
        powerBuffFlag = m_powerBuffFlag;
		return powerBuffFlag;
    }
    bool GetAttackSpeedBuffFlag(bool attackSpeedBuffFlag)
    {
        attackSpeedBuffFlag = m_attackSpeedBuffFlag;
		return attackSpeedBuffFlag;
	}
    void Render(RenderContext& rc)override;


private:
    //メンバ変数
 //   enum EnAnimationClip {		//アニメーション。
 //       enAnimationClip_Idle,
 //       enAnimationClip_Walk,
 //       enAnimationClip_Jump,
 //       enAnimationClip_Run,
 //       enAnimationClip_Num,
 //   };
	//AnimationClip m_animationClips[enAnimationClip_Num];
    ModelRender m_modelRender;
    CharacterController m_characterController;
    Vector3 m_moveSpeed;
    Quaternion m_rotation;
    Vector3 m_position;
    CollisionObject* m_collisionObject;
    Vector3 m_forward;
    Vector3 m_knockBack;
	Gire* m_gire = nullptr;
    Game* m_game = nullptr;
	SoundSource* m_se;
    int m_playerHp = 100;				//!<プレイヤーのHP。
	int m_playerMaxHp = 100;				//!<プレイヤーの最大HP。
    int m_playerState = 0;
	int m_attackPower = 0;				//!<攻撃力。
    int m_heal = 20;
	float m_timeCount = 0.0f;				//!<タイマー用の変数。
	float m_damageIntarvalTime = 0.0f;		//!<ダメージを受けてからの無敵時間。
	float m_guardTime = 0.0f;				//!<ガードしている時間。
	float m_guardIntervalTime = 0.0f;			//!<ガード後のクールタイム。
	float m_powerBuffTime = 0.0f;				//!<攻撃力バフの時間。
	float m_attackSpeedBuffTime = 0.0f;		//!<攻撃速度バフの時間。
    int m_gireCount;
	bool m_isGetGire = false;				//!<ギアを取ったかどうか。
    bool m_guardFlag = false;
    bool m_powerBuffFlag = false;
    bool m_attackSpeedBuffFlag = false;
};

