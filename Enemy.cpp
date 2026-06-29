#include "Enemy.h"
#include "time.h"
#include "Player.h"
#include <cmath>
#include <DirectXMath.h>

namespace
{
	const int ENEMY_SIZE = 48;
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32;
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;

	const float ENEMY_MOVE_INTERVAL = 0.2f;
	const float ENEMY_CHASE_INTERVAL = 0.15f;

	const int VIEW_DIST = 5;
	const float VIEW_ANGLE = 45.0f;

	const int ATTACK_DIST = 1;        // 攻撃可能マンハッタン距離（タイル）
	const float SEARCH_TIME = 10.0f;   // 探索タイマー（秒）
}


Enemy::Enemy()
	: GameObject()
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	// 外周の上辺からランダムにスポーン
	int startX = GetRand(STAGE_WIDTH - 3) + 1;
	pos_ = { startX * ENEMY_DRAW_SIZE, 1 * ENEMY_DRAW_SIZE };
	dir_ = INIT_ENEMY_DIR;
	state_ = ESTATE::PATROL;
	stateTimer_ = 0.0f;
	moveTimer_ = 0.0f;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	// 視界タイルを毎フレーム更新
	viewTiles_ = GetViewTiles(VIEW_ANGLE, VIEW_DIST);

	switch (state_)
	{
	case ESTATE::PATROL: UpdatePatrol(); break;
	case ESTATE::CHASE:  UpdateChase();  break;
	case ESTATE::ATTACK: UpdateAttack(); break;
	case ESTATE::SEARCH: UpdateSearch(); break;
	default: break;
	}
}

// --- ヘルパー: dir_方向に1マス移動。移動できたらtrue ---
bool Enemy::MoveOneStep()
{
	const Point delta[4] = { {0,-1},{0,1},{-1,0},{1,0} };
	Point d = delta[dir_];
	Point newPos = { pos_.x + d.x * ENEMY_DRAW_SIZE, pos_.y + d.y * ENEMY_DRAW_SIZE };
	if (newPos.x < 1 || newPos.x >(STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE ||
		newPos.y < 1 || newPos.y >(STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE)
	{
		return false;
	}
	pos_ = newPos;
	return true;
}

// --- 外周を時計回りに巡回 ---
void Enemy::UpdatePatrol()
{
	Player* p = FindGameObject<Player>();
	if (CanSeePlayer(p))
	{
		state_ = ESTATE::CHASE;
		printfDx("STATE CHANGE->CHASE!\n");
		return;
	}

	moveTimer_ -= Time::DeltaTime();
	if (moveTimer_ <= 0.0f)
	{
		if (!MoveOneStep())
		{
			TurnRight();
		}
		moveTimer_ = ENEMY_MOVE_INTERVAL;
	}
}

// --- プレイヤーに近づく ---
void Enemy::UpdateChase()
{
	Player* p = FindGameObject<Player>();
	Point pPos = p->GetPlayerPos();

	// マンハッタン距離がATTACK_DIST以内→ATTACK
	int mDist = (std::abs(pPos.x - pos_.x) + std::abs(pPos.y - pos_.y)) / ENEMY_DRAW_SIZE;
	if (mDist <= ATTACK_DIST)
	{
		state_ = ESTATE::ATTACK;
		printfDx("STATE CHANGE->ATTACK!\n");
		return;
	}

	// プレイヤーの方向を向く
	int xDist = std::abs(pPos.x - pos_.x);
	int yDist = std::abs(pPos.y - pos_.y);
	if (xDist >= yDist)
		dir_ = (pPos.x > pos_.x) ? DIR::RIGHT : DIR::LEFT;
	else
		dir_ = (pPos.y > pos_.y) ? DIR::DOWN : DIR::UP;

	moveTimer_ -= Time::DeltaTime();
	if (moveTimer_ <= 0.0f)
	{
		MoveOneStep();
		moveTimer_ = ENEMY_CHASE_INTERVAL;
	}
}

// --- 攻撃 ---
void Enemy::UpdateAttack()
{
	Player* p = FindGameObject<Player>();
	Point pPos = p->GetPlayerPos();

	// プレイヤーを向く
	int xDist = std::abs(pPos.x - pos_.x);
	int yDist = std::abs(pPos.y - pos_.y);
	if (xDist >= yDist)
		dir_ = (pPos.x > pos_.x) ? DIR::RIGHT : DIR::LEFT;
	else
		dir_ = (pPos.y > pos_.y) ? DIR::DOWN : DIR::UP;

	// 攻撃距離から逃げられたらSEARCH
	int mDist = (xDist + yDist) / ENEMY_DRAW_SIZE;
	if (mDist > ATTACK_DIST)
	{
		state_ = ESTATE::SEARCH;
		stateTimer_ = SEARCH_TIME;
		printfDx("STATE CHANGE->SEARCH!\n");
	}
}

// --- 探索 ---
void Enemy::UpdateSearch()
{
	stateTimer_ -= Time::DeltaTime();
	if (stateTimer_ < 0)
	{
		state_ = ESTATE::PATROL;
		printfDx("STATE CHANGE->PATROL!\n");
		return;
	}

	Player* p = FindGameObject<Player>();
	if (CanSeePlayer(p))
	{
		state_ = ESTATE::ATTACK;
		printfDx("STATE CHANGE->ATTACK!\n");
		return;
	}

	// うろうろ：移動できなかったらランダムに方向転換
	moveTimer_ -= Time::DeltaTime();
	
	if (moveTimer_ <= 0.0f)
	{
		MoveOneStep();
		dir_ = (DIR)(GetRand(3));//壁だろうが、なんだろうが動く時間になったらどっかに動く＝探す
		moveTimer_ = ENEMY_MOVE_INTERVAL;
	}
}

/// <summary>
std::vector<Point> Enemy::GetViewTiles(float angle, int dist)
{
	std::vector<Point> viewTiles;
	Point dVec[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
	Point pPos = { pos_.x / ENEMY_DRAW_SIZE, pos_.y / ENEMY_DRAW_SIZE };
	float rad = DirectX::XMConvertToRadians(angle);

	for (int dy = -dist; dy <= dist; dy++) {
		for (int dx = -dist; dx <= dist; dx++) {
			if (dx == 0 && dy == 0) continue;
			if (sqrt(dx * dx + dy * dy) > dist) continue;
			Point face = dVec[dir_];
			DirectX::XMVECTOR faceVec = DirectX::XMVector2Normalize(DirectX::XMVectorSet(face.x, face.y, 0.0f, 0.0f));
			DirectX::XMVECTOR dirVec  = DirectX::XMVector2Normalize(DirectX::XMVectorSet(dx, dy, 0.0f, 0.0f));
			float dot = DirectX::XMVectorGetX(DirectX::XMVector2Dot(faceVec, dirVec));
			if (dot < cosf(rad)) continue;
			Point viewTile = { pPos.x + dx, pPos.y + dy };
			if (viewTile.x < 0 || viewTile.x >= STAGE_WIDTH ||
				viewTile.y < 0 || viewTile.y >= STAGE_HEIGHT) continue;
			viewTiles.push_back(viewTile);
		}
	}
	return viewTiles;
}



void Enemy::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	Rect iRect[4] = {
		{  nowFrame * ENEMY_SIZE, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE}
	};
	DrawBox(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0), FALSE,2);
	DrawRectExtendGraph(pos_.x, pos_.y,pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		               iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);
	
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	for (auto& tile : viewTiles_)
	{
		DrawBox(tile.x * CHA_SIZE, tile.y * CHA_SIZE,
			(tile.x + 1) * CHA_SIZE, (tile.y + 1) * CHA_SIZE,
			GetColor(0, 255, 0), TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	animTimer -= Time::DeltaTime();
	if (animTimer < 0) {
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}
}

void Enemy::TurnLeft()
{
	switch(dir_)
	{
	case   UP:
		dir_ = DIR::LEFT;
		break;
	case DOWN:
		dir_ = DIR::RIGHT;
		break;
	case LEFT:
		dir_ = DIR::DOWN;
		break;
	case RIGHT:
		dir_ = DIR::UP;
		break;
	default:
		break;
	}
}

void Enemy::TurnRight()
{
	switch (dir_)
	{
	case   UP:
		dir_ = DIR::RIGHT;
			break;
	case DOWN:
		dir_ = DIR::LEFT;
			break;
	case LEFT:
		dir_ = DIR::UP;
			break;
	case RIGHT:
		dir_ = DIR::DOWN;
			break;
	default:
		break;
	}
}

void Enemy::TurnBack()
{
	switch (dir_)
	{
	case   UP:
		dir_ = DIR::DOWN;
			break;
	case DOWN:
		dir_ = DIR::UP;
			break;
	case LEFT:
		dir_ = DIR::RIGHT;
			break;
	case RIGHT:
		dir_ = DIR::LEFT;
			break;
	default:
		break;
	}
}

bool Enemy::CanSeePlayer(Player* p)
{
	Point pTile = { p->GetPlayerPos().x / ENEMY_DRAW_SIZE, p->GetPlayerPos().y / ENEMY_DRAW_SIZE };
	for (auto& tile : viewTiles_)
	{
		if (tile.x == pTile.x && tile.y == pTile.y) return true;
	}

	return false;
}

