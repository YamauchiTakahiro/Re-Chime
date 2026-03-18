#pragma once

#include "Level3DRender/LevelRender.h"


class Player;
class GameCamera;
class Stage;

class Title;
class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	Player* m_player = nullptr;
	GameCamera* m_gameCamera = nullptr;
	Stage* m_stage = nullptr;
	Title* m_title = nullptr;
};

