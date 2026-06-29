#include "Enemy.h"
#include "time.h"
#include "Player.h"
#include <cmath>
#include <DirectXMath.h>

namespace
{
	const int ENEMY_SIZE = 48; 
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //?G???????u
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //?G??`??T?C?Y
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;

	const float ENEMY_MOVE_INTERVAL = 0.2f; 
	const float ENEMY_CHASE_INTERVAL = 0.15f;

	const int THRESHOLD_DIST = 5;
	const float INIT_CHASE_TIME = 5.0; 

	const int VIEW_DIST = 5;
	const float VIEW_ANGLE = 45.0f;
}


Enemy::Enemy()
	: GameObject() 
{
	hImage_ = LoadGraph("Assets/panda_R.png");

	Point rndNum = { GetRand(STAGE_WIDTH - 2) + 1, GetRand(STAGE_HEIGHT - 2) + 1 };
	pos_ = {rndNum.x * ENEMY_DRAW_SIZE, rndNum.y * ENEMY_DRAW_SIZE};
	dir_ = INIT_ENEMY_DIR;
	state_ = ESTATE::NORMAL;
	chaseTime_ = INIT_CHASE_TIME;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	switch (state_)
	{
	case ESTATE::CHASE:
		UpdateChase();
		break;
	case ESTATE::ESCAPE:
		UpdateEscape();
		break;
	default:
		return;
	}

}

//void Enemy::UpdateNormal()
//{
//
////static float dir_timer = 3.0f;
//	static float prog_timer = ENEMY_MOVE_INTERVAL;
//	float dt = Time::DeltaTime();
//	//dir_timer = dir_timer - dt;
//	prog_timer = prog_timer - dt;
//	//if (dir_timer < 0.0f)
//	//{
//	//	//dir_ = (DIR)(GetRand(3));
//	//	//TurnLeft();
//	//	TurnBack();
//	//	dir_timer = 3.0f + dir_timer;
//	//}
//
//	Point newPos = pos_;
//	if (prog_timer < 0.0f)
//	{
//		switch (dir_)
//		{
//		case UP:
//			newPos.y -= ENEMY_DRAW_SIZE;
//			break;
//		case DOWN:
//			newPos.y += ENEMY_DRAW_SIZE;
//			break;
//		case LEFT:
//			newPos.x -= ENEMY_DRAW_SIZE;
//			break;
//		case RIGHT:
//			newPos.x += ENEMY_DRAW_SIZE;
//			break;
//		default:
//			break;
//		}
//
//		if (!(newPos.x < 1 || newPos.x >(STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE
//			|| newPos.y < 1 || newPos.y >(STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE))
//		{
//			pos_ = newPos;
//
//		}
//		else
//		{
//			TurnLeft();
//		}
//
//		Player* p = FindGameObject<Player>();
//		viewTiles_ = GetViewTiles(VIEW_ANGLE, VIEW_DIST);
//		if (CanSeePlayer(p))
//		{
//			state_ = ESTATE::CHASE;
//			printfDx("STATE CHANGE->CHASE!\n");
//		}
//
//
//		prog_timer = ENEMY_MOVE_INTERVAL + prog_timer;
//	}
//}

void Enemy::UpdateChase()
{
	Player* p = FindGameObject<Player>();
	Point pPos = p->GetPlayerPos();

	Point arrPos = { pPos.x / ENEMY_DRAW_SIZE, pPos.y / ENEMY_DRAW_SIZE };
	if (arrPos.x == 18 && arrPos.y == 10)
	{
		state_ = ESTATE::ESCAPE;
		printfDx("STATE CHANGE->ESCAPE!\n");
		chaseTime_ = INIT_CHASE_TIME;
		return;
	}


	float dt = Time::DeltaTime();
	if (chaseTime_ < 0)
	{
		state_ = ESTATE::NORMAL;
		chaseTime_ = INIT_CHASE_TIME;
		printfDx("STATE CHANGE->NORMAL!\n");
	}
	else {


		int xDist = abs(pPos.x - pos_.x);
		int yDist = abs(pPos.y - pos_.y);
		if (xDist > yDist) 
		{
			if (pPos.x > pos_.x) {
				dir_ = DIR::RIGHT;
			}
			else {
				dir_ = DIR::LEFT;
			}
		}
		else if (xDist < yDist)
		{
			if (pPos.y > pos_.y) {
				dir_ = DIR::DOWN;
			}
			else {
				dir_ = DIR::UP;
			}
		}

		static float prog_timer = ENEMY_CHASE_INTERVAL;
		Point newPos = pos_;
		if (prog_timer < 0.0f)
		{
			switch (dir_)
			{
			case UP:
				newPos.y -= ENEMY_DRAW_SIZE;
				break;
			case DOWN:
				newPos.y += ENEMY_DRAW_SIZE;
				break;
			case LEFT:
				newPos.x -= ENEMY_DRAW_SIZE;
				break;
			case RIGHT:
				newPos.x += ENEMY_DRAW_SIZE;
				break;
			default:
				break;
			}

			if (!(newPos.x < 1 || newPos.x >(STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE
				|| newPos.y < 1 || newPos.y >(STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE))
			{
				pos_ = newPos;

			}
			prog_timer = ENEMY_CHASE_INTERVAL + prog_timer;
		}
		prog_timer = prog_timer - dt;
		chaseTime_ = chaseTime_ - dt;
	}
}

void Enemy::UpdateEscape()
{
	static float prog_timer = ENEMY_MOVE_INTERVAL;
	float dt = Time::DeltaTime();
	chaseTime_ = chaseTime_ - dt;
	
	if (chaseTime_ < 0)
	{
		state_ = ESTATE::NORMAL;
		chaseTime_ = INIT_CHASE_TIME;
		printfDx("STATE CHANGE->NORMAL!\n");
		return;
	}
	Player* p = FindGameObject<Player>();
	Point pPos = p->GetPlayerPos();

	Point dist = { std::abs(pPos.x - pos_.x), std::abs(pPos.y - pos_.y) };
	if (dist.x > dist.y)
	{
		if (pPos.x > pos_.x) {
			dir_ = DIR::LEFT;
		}
		else {
			dir_ = DIR::RIGHT;
		}
	}
	else if (dist.x < dist.y)
	{
		if (pPos.y > pos_.y) {
			dir_ = DIR::UP;
		}
		else {
			dir_ = DIR::DOWN;
		}
	}

	prog_timer = prog_timer - dt;
	if (prog_timer < 0.0f)
	{
		Point newPos = pos_;
		switch (dir_)
		{
		case UP:
			newPos.y -= ENEMY_DRAW_SIZE;
			break;
		case DOWN:
			newPos.y += ENEMY_DRAW_SIZE;
			break;
		case LEFT:
			newPos.x -= ENEMY_DRAW_SIZE;
			break;
		case RIGHT:
			newPos.x += ENEMY_DRAW_SIZE;
			break;
		default:
			break;
		}
		//???????X?e?[?W??O??o??????????
		if (!(newPos.x < 1 || newPos.x >(STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE
			|| newPos.y < 1 || newPos.y >(STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE))
		{
			pos_ = newPos;
		}
		prog_timer = ENEMY_MOVE_INTERVAL + prog_timer;

	}

}

void Enemy::DrawFieldOfViewArc_PureDxLib(
	float fovAngleDeg,
	int   viewDistanceTiles,
	int   numSegments
) const {
	// 1) ???a??s?N?Z??????
	float radius = viewDistanceTiles * CHA_SIZE;

	// 2) ???S???W?i?G????S?j
	float cx = pos_.x + CHA_SIZE * 0.5f;
	float cy = pos_.y + CHA_SIZE * 0.5f;

	// 3) ???? ?? ?x?[?X?p?x?i?x?????W?A???j
	float baseDeg = 0.0f;
	switch (dir_) {
	case DIR::RIGHT: baseDeg = 0.0f; break;
	case DIR::UP:    baseDeg = -90.0f; break;
	case DIR::LEFT:  baseDeg = 180.0f; break;
	case DIR::DOWN:  baseDeg = 90.0f; break;
	}
	float halfRad = (fovAngleDeg * 0.5f) * (3.14159f / 180.0f);
	float startRad = (baseDeg * (3.14159f / 180.0f)) - halfRad;
	float endRad = (baseDeg * (3.14159f / 180.0f)) + halfRad;

	// 4) ?h?????
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 96);
	for (int i = 0; i < numSegments; ++i) {
		float t0 = i / float(numSegments);
		float t1 = (i + 1) / float(numSegments);
		float a0 = startRad + (endRad - startRad) * t0;
		float a1 = startRad + (endRad - startRad) * t1;

		float x0 = cx + cosf(a0) * radius;
		float y0 = cy + sinf(a0) * radius;
		float x1 = cx + cosf(a1) * radius;
		float y1 = cy + sinf(a1) * radius;

		DrawTriangle(
			int(cx), int(cy),
			int(x0), int(y0),
			int(x1), int(y1),
			GetColor(255, 255, 0),
			TRUE
		);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 5) ??s??
	const int lineColor = GetColor(255, 255, 0);
	float prevX = cx + cosf(startRad) * radius;
	float prevY = cy + sinf(startRad) * radius;
	for (int i = 1; i <= numSegments; ++i) {
		float t = i / float(numSegments);
		float ang = startRad + (endRad - startRad) * t;
		float nx = cx + cosf(ang) * radius;
		float ny = cy + sinf(ang) * radius;
		DrawLine(int(prevX), int(prevY), int(nx), int(ny), lineColor);
		prevX = nx; prevY = ny;
	}
	// ???[?????S????
	float xA = cx + cosf(startRad) * radius;
	float yA = cy + sinf(startRad) * radius;
	float xB = cx + cosf(endRad) * radius;
	float yB = cy + sinf(endRad) * radius;
	DrawLine(int(cx), int(cy), int(xA), int(yA), lineColor);
	DrawLine(int(cx), int(cy), int(xB), int(yB), lineColor);
}

/// <summary>
/// ?w?????p?x????????????A?G????E???^?C????v?Z??????B
/// </summary>
/// <param name="angle">????p(?x?P??)?B</param>
/// <param name="dist">???E???????(?^?C???P??)?B</param>
/// <returns>???E???^?C?????W??x?N?^?[?B</returns>
std::vector<Point> Enemy::GetViewTiles(float angle, int dist)
{
	std::vector<Point> viewTiles;//???E??^?C????i?[????x?N?^?[
	Point dVec[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
	Point pPos = { pos_.x / ENEMY_DRAW_SIZE, pos_.y / ENEMY_DRAW_SIZE };

	//float rad = angle * (3.14159f / 180.0f); //?x????W?A??????
	float rad =  DirectX::XMConvertToRadians(angle);

	for (int dy = -dist; dy <= dist; dy++){
		for (int dx = -dist; dx <= dist; dx++){
			if (dx == 0 && dy == 0) continue; //????????O
			//?????`?F?b?N
			if (sqrt(dx * dx + dy * dy) > dist) continue; //??????dist??????????O
			Point face = dVec[dir_];
			DirectX::XMVECTOR faceVec = DirectX::XMVectorSet(face.x, face.y, 0.0f, 0.0f);
			DirectX::XMVECTOR dirVec = DirectX::XMVectorSet(dx, dy, 0.0f, 0.0f);
			faceVec = DirectX::XMVector2Normalize(faceVec);
			dirVec = DirectX::XMVector2Normalize(dirVec);
			//DirectX::XMVector2Dot(faceVec, dirVec);
			float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector2Dot(faceVec, dirVec));
			//?p?x?`?F?b?N
			if (dotProduct < cosf(rad)) continue; //?p?x??rad???????????O
			//???E??^?C??????
			Point viewTile = { pPos.x + dx, pPos.y + dy };
			//?X?e?[?W??O??o??????????
			if (viewTile.x < 0 || viewTile.x >= STAGE_WIDTH ||
				viewTile.y < 0 || viewTile.y >= STAGE_HEIGHT) continue;
			//?^?C??????	
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
	
	//???E??`??
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	for (auto& tile : viewTiles_)
	{
		DrawBox(tile.x * CHA_SIZE, tile.y * CHA_SIZE,
			(tile.x + 1) * CHA_SIZE, (tile.y + 1) * CHA_SIZE,
			GetColor(0, 255, 0), TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	
	if (animTimer < 0) {
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}
	//animTimer = animTimer - Time::DeltaTime();
	//if (state_ == ESTATE::CHASE)
	//{
	//	printfDx("CHASE!\n");
	//}
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
	//dir_?@?p???_?????
	//?O??5?}?X????E?????A?????????????h??I
	//VIEW_DIST = 5?????????
	//Point playerP = p->GetPlayerPos();
	//playerP = { playerP.x / CHA_SIZE, playerP.y / CHA_SIZE };
	//Point pPos = { pos_.x / CHA_SIZE, pos_.y / CHA_SIZE };
	//
	//Point dVec[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

	//for (int i = 1; i <= VIEW_DIST; i++) {
	//	Point posTmp = { pPos.x + i * dVec[dir_].x , pPos.y + i * dVec[dir_].y
	//};
	//	if (playerP.x == posTmp.x && playerP.y == posTmp.y)
	//		return true;
	//}

	Point pTile = { p->GetPlayerPos().x / ENEMY_DRAW_SIZE, p->GetPlayerPos().y / ENEMY_DRAW_SIZE };
	for (auto& tile : viewTiles_)
	{
		if (tile.x == pTile.x && tile.y == pTile.y) return true;
	}

	return false;
}

