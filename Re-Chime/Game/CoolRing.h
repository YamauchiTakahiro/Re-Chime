#pragma once
class CoolRing
{
public:
    void Init();
    void SetPosition(const Vector3& pos);
    void SetScale(const Vector3& scale);

    void SetProgress(float rate);
    void Draw(RenderContext& rc);

private:
    static const int RING_MAX = 9;

    SpriteRender m_ring[RING_MAX];

    int m_nowRing = 0;
    Vector3 m_pos;
    Vector3 m_scale;
};

