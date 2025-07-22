#include "Enemy.h"
#include "time.h"
#include "Player.h"
#include <cmath>
#include <DirectXMath.h>

namespace
{
	const int ENEMY_SIZE = 48; //敵のサイズ 32*32
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;

	const float ENEMY_MOVE_INTERVAL = 0.2f; //敵の移動間隔
	const float ENEMY_CHASE_INTERVAL = 0.15f; //敵の追いかける間隔

	const int THRESHOLD_DIST = 5;
	const float INIT_CHASE_TIME = 5.0; //10s追いかける

	const int VIEW_DIST = 5;
	const float VIEW_ANGLE = 45.0f;
}


Enemy::Enemy()
	: GameObject() 
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	//pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	//↑ステージ内のランダム位置にスポーン
	// 39x21
	//  幅で見ると　0,39が石のなか　ステージは　1～38
	//  高で見ると  0,21が石のなか　ステージは  1～20
	//GetRand(x)は　0～xまでの整数が発生　1～３８，1～２０の乱数を出すには？
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
	//プレイヤーが、10,15 にいたら、escapeモード
	switch (state_)
	{
	case ESTATE::NORMAL:
		UpdateNormal();
		break;
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

void Enemy::UpdateNormal()
{
	//GetRand(数値)
//3秒に1回向きをランダムに変える
//static float dir_timer = 3.0f;
	static float prog_timer = ENEMY_MOVE_INTERVAL;
	float dt = Time::DeltaTime();
	//dir_timer = dir_timer - dt;
	prog_timer = prog_timer - dt;
	//if (dir_timer < 0.0f)
	//{
	//	//dir_ = (DIR)(GetRand(3));
	//	//TurnLeft();
	//	TurnBack();
	//	dir_timer = 3.0f + dir_timer;
	//}

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
		//移動先がステージの外に出ないようにする
		if (!(newPos.x < 1 || newPos.x >(STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE
			|| newPos.y < 1 || newPos.y >(STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE))
		{
			pos_ = newPos;

		}
		else
		{
			TurnLeft();
		}

		Player* p = FindGameObject<Player>();
		//Point pPos = p->GetPlayerPos();
		//プレイヤーとのマンハッタン距離がTHRESHOLD_DISTより小さくなったら
		//CHASE
			// マンハッタン距離の計算: |x1 - x2| + |y1 - y2|
		//int distance = (std::abs(pPos.x - pos_.x) +
		//	std::abs(pPos.y - pos_.y))/ ENEMY_DRAW_SIZE;
		//if (distance <= THRESHOLD_DIST) {
		//	state_ = ESTATE::CHASE;
		//	printfDx("STATE CHANGE->CHASE!\n");
		//}
		//センサーがあったら
		if (CanSeePlayer(p))
		{
			state_ = ESTATE::CHASE;
			printfDx("STATE CHANGE->CHASE!\n");
		}


		prog_timer = ENEMY_MOVE_INTERVAL + prog_timer;
	}
}

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

	//10秒おいかけてNORMALに戻る
	float dt = Time::DeltaTime();
	if (chaseTime_ < 0)
	{
		state_ = ESTATE::NORMAL;
		chaseTime_ = INIT_CHASE_TIME;
		printfDx("STATE CHANGE->NORMAL!\n");
	}
	else {
		//マンハッタン距離を減らそう！

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
			//移動先がステージの外に出ないようにする
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
		//移動先がステージの外に出ないようにする
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
	// 1) 半径をピクセルに変換
	float radius = viewDistanceTiles * CHA_SIZE;

	// 2) 中心座標（敵の中心）
	float cx = pos_.x + CHA_SIZE * 0.5f;
	float cy = pos_.y + CHA_SIZE * 0.5f;

	// 3) 向き → ベース角度（度→ラジアン）
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

	// 4) 塗りつぶし
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

	// 5) 輪郭線
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
	// 両端から中心への線
	float xA = cx + cosf(startRad) * radius;
	float yA = cy + sinf(startRad) * radius;
	float xB = cx + cosf(endRad) * radius;
	float yB = cy + sinf(endRad) * radius;
	DrawLine(int(cx), int(cy), int(xA), int(yA), lineColor);
	DrawLine(int(cx), int(cy), int(xB), int(yB), lineColor);
}

std::vector<Point> Enemy::GetViewTiles(float angle, int dist)
{
	std::vector<Point> viewTiles;//視界のタイルを格納するベクター
	Point dVec[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
	Point pPos = { pos_.x / ENEMY_DRAW_SIZE, pos_.y / ENEMY_DRAW_SIZE };

	//float rad = angle * (3.14159f / 180.0f); //度をラジアンに変換
	float rad =  DirectX::XMConvertToRadians(angle);

	for (int dy = -dist; dy <= dist; dy++){
		for (int dx = -dist; dx <= dist; dx++){
			if (dx == 0 && dy == 0) continue; //自分は除外
			//距離チェック
			if (sqrt(dx * dx + dy * dy) > dist) continue; //距離がdistより大きいなら除外
			Point face = dVec[dir_];
			DirectX::XMVECTOR faceVec = DirectX::XMVectorSet(face.x, face.y, 0.0f, 0.0f);
			DirectX::XMVECTOR dirVec = DirectX::XMVectorSet(dx, dy, 0.0f, 0.0f);
			faceVec = DirectX::XMVector2Normalize(faceVec);
			dirVec = DirectX::XMVector2Normalize(dirVec);
			//DirectX::XMVector2Dot(faceVec, dirVec);
			float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector2Dot(faceVec, dirVec));
			//角度チェック
			if (dotProduct < cosf(rad)) continue; //角度がradより小さいなら除外
			//視界のタイルを追加
			Point viewTile = { pPos.x + dx, pPos.y + dy };
			//ステージの外に出ないようにする
			if (viewTile.x < 0 || viewTile.x >= STAGE_WIDTH ||
				viewTile.y < 0 || viewTile.y >= STAGE_HEIGHT) continue;
			//タイルを追加	
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
	
	//視界の描画
	Point fdir[4] = { {0,-1},{0,1},{-1,0},{1,0} };
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);

	//DrawFieldOfViewArc_PureDxLib(90, 3, 10);
	auto&& viewTiles = GetViewTiles(VIEW_ANGLE, VIEW_DIST);
	for (int i = 0; i < viewTiles.size(); i++)
	{
		Point tile = viewTiles[i];
		int x = tile.x * CHA_SIZE;
		int y = tile.y * CHA_SIZE;
		DrawBox(x, y, x + CHA_SIZE, y + CHA_SIZE, GetColor(0, 255, 0), TRUE);
	}
	//塗る
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
	//dir_　パンダの向き
	//前方5マスを視界として、緑か青の半透明で塗る！
	//VIEW_DIST = 5に設定したよね
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

	auto&& viewTiles = GetViewTiles(VIEW_ANGLE, VIEW_DIST);
	for (int i = 0; i < viewTiles.size(); i++)
	{
		Point tile = viewTiles[i];
		if (tile.x == p->GetPlayerPos().x / ENEMY_DRAW_SIZE &&
			tile.y == p->GetPlayerPos().y / ENEMY_DRAW_SIZE)
		{
			return true;
		}
	}

	return false;
}

