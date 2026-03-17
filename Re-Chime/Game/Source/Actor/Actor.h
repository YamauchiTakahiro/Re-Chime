#pragma once
class Actor : public IGameObject
{
public:
	virtual bool Start() override;
	virtual void Update() override;
	virtual void Render(RenderContext& rc)override;
};

